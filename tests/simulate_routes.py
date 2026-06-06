#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
StoryEngine 行为级模拟器 + 5 条路径走通测试
================================================
完整复刻 src/story/StoryEngine.cpp 的逻辑：
  - 节点跳转、END 处理、空 choices 死锁检测
  - 节点级 condition 失败 → 整本剧本结束（与 jumpToNode L78-83 一致）
  - 选项级 condition 失败 → 隐藏该选项（与 currentChoices L150 一致）
  - effects: affinity.X / stress / darkness, 数值夹紧 [0, 100]
  - condition 求值: && || > >= < <= == != , 优先级 || > &&
目标：把 4 条主攻线 + 1 条平衡线从 week1 → week5 走通，确认无死路、无断链、
      最终结局判定符合设计。
"""

import json
import re
import os
import sys
from collections import OrderedDict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SCRIPT_DIR = os.path.join(ROOT, 'assets', 'scripts')

# ---------- Player 状态（mock 自 Player.cpp）----------
class Player:
    def __init__(self):
        self.affinity = {'ProgDesign': 0, 'Calculus': 0, 'LinearAlgebra': 0, 'AIIntro': 0}
        self.stress = 0
        self.darkness = 0
        self.week = 1
        self.day = 1

    def apply_effects(self, effects):
        for eff in effects or []:
            target = eff['target']
            delta = eff['delta']
            if target.startswith('affinity.'):
                k = target[len('affinity.'):]
                if k in self.affinity:
                    self.affinity[k] = max(0, min(100, self.affinity[k] + delta))
            elif target == 'stress':
                self.stress = max(0, min(100, self.stress + delta))
            elif target == 'darkness':
                self.darkness = max(0, min(100, self.darkness + delta))

    def read(self, name):
        if name == 'stress': return self.stress
        if name == 'darkness': return self.darkness
        if name == 'week': return self.week
        if name == 'day': return self.day
        if name.startswith('affinity.'):
            return self.affinity.get(name[len('affinity.'):], 0)
        return 0

    def snapshot(self):
        return f"PD={self.affinity['ProgDesign']:3d} CA={self.affinity['Calculus']:3d} " \
               f"LA={self.affinity['LinearAlgebra']:3d} AI={self.affinity['AIIntro']:3d} " \
               f"S={self.stress:3d} D={self.darkness:3d}"

# ---------- condition 求值（与 StoryEngine::evaluateCondition 一致）----------
ATOMIC_RE = re.compile(r'^\s*([a-zA-Z][\w.]*)\s*(>=|<=|==|!=|>|<)\s*(-?\d+)\s*$')

def eval_atomic(expr, player):
    m = ATOMIC_RE.match(expr)
    if not m: return True
    var, op, rhs = m.group(1), m.group(2), int(m.group(3))
    lhs = player.read(var)
    return {'>':lhs>rhs, '>=':lhs>=rhs, '<':lhs<rhs, '<=':lhs<=rhs,
            '==':lhs==rhs, '!=':lhs!=rhs}[op]

def eval_cond(expr, player):
    if not expr or not expr.strip(): return True
    # || 优先级最低
    if '||' in expr:
        return any(eval_cond(p, player) for p in expr.split('||'))
    if '&&' in expr:
        return all(eval_cond(p, player) for p in expr.split('&&'))
    return eval_atomic(expr, player)

# ---------- 剧本运行器 ----------
class ScriptRunner:
    def __init__(self, path, player, trace=False):
        with open(path) as f:
            self.doc = json.load(f)
        self.nodes = {n['id']: n for n in self.doc['nodes']}
        self.player = player
        self.trace = trace
        self.name = os.path.basename(path)
        self.path_log = []  # 走过的节点 id 序列

    def visible_choices(self, node):
        """复刻 currentChoices: 过滤掉 condition 不满足的选项"""
        out = []
        for i, c in enumerate(node.get('choices', [])):
            cond = c.get('condition', '')
            if cond and not eval_cond(cond, self.player):
                continue
            out.append((i, c))
        return out

    def run(self, choice_strategy):
        """choice_strategy(node, visible_choices, player) -> idx  返回要选的 visible choice 下标"""
        nid = self.doc['startNode']
        while True:
            if nid == 'END' or not nid:
                if self.trace: self.path_log.append('[END]')
                return 'END'
            node = self.nodes.get(nid)
            if node is None:
                return f'[BROKEN: node {nid} not found]'
            # 节点级 condition（与引擎一致：失败 → emit scriptFinished）
            cond = node.get('condition', '')
            if cond and not eval_cond(cond, self.player):
                if self.trace: self.path_log.append(f'{nid}[cond_fail]')
                return f'[NODE_COND_FAIL: {nid}]'
            self.player.apply_effects(node.get('effects'))
            self.path_log.append(nid)

            visible = self.visible_choices(node)
            if not visible:
                # 没有可见选项 → 对话死锁
                return f'[DEAD_END: {nid} has no visible choices]'

            idx = choice_strategy(node, visible, self.player)
            if idx is None or idx >= len(visible):
                return f'[INVALID_CHOICE at {nid}]'
            _, chosen = visible[idx]
            self.player.apply_effects(chosen.get('effects'))
            nid = chosen.get('next', 'END')

# ---------- 策略：每条主攻线对应的"贪心"选项选择器 ----------
def make_max_subject_strategy(target_subject):
    """优先选能让 target_subject 好感度涨最多的选项；其次正向加好感；最后看 stress -"""
    def strategy(node, visible, player):
        best_idx, best_score = 0, -10**9
        target_key = f'affinity.{target_subject}'
        for vi, (_, c) in enumerate(visible):
            score = 0
            for eff in c.get('effects', []) or []:
                if eff['target'] == target_key:
                    score += 100 * eff['delta']
                elif eff['target'].startswith('affinity.'):
                    score += 2 * eff['delta']
                elif eff['target'] == 'stress':
                    score -= eff['delta']  # 压力少为佳
                elif eff['target'] == 'darkness':
                    # 线代线允许加，其他线避免
                    if target_subject == 'LinearAlgebra':
                        score += 3 * eff['delta']
                    else:
                        score -= 2 * eff['delta']
            if score > best_score:
                best_score, best_idx = score, vi
        return best_idx
    return strategy

def balanced_strategy(node, visible, player):
    """默认/平衡线：优先选不会减好感、压力适中的"""
    best_idx, best_score = 0, -10**9
    for vi, (_, c) in enumerate(visible):
        score = 0
        for eff in c.get('effects', []) or []:
            if eff['target'].startswith('affinity.'):
                score += eff['delta']  # 雨露均沾
            elif eff['target'] == 'stress':
                score -= abs(eff['delta'])
            elif eff['target'] == 'darkness':
                score -= 5 * eff['delta']  # 平衡线避免黑化
        if score > best_score:
            best_score, best_idx = score, vi
    return best_idx

# ---------- 跑一条完整路径 ----------
def play_path(label, strategy, route_file=None, ending_guess=None):
    print(f"\n{'='*78}\n  路径: {label}\n{'='*78}")
    player = Player()
    log_lines = []

    # 1. 跑 week1 ~ week5
    for w in range(1, 6):
        player.week = w
        f = os.path.join(SCRIPT_DIR, f'week{w}.json')
        runner = ScriptRunner(f, player)
        result = runner.run(strategy)
        nodes_visited = len(runner.path_log)
        log_lines.append(f"  week{w}.json: {nodes_visited:>3} 节点 → {result:<30} | {player.snapshot()}")
        if not result.startswith('END'):
            print('\n'.join(log_lines))
            print(f"  ❌ 在 week{w} 出错：{result}")
            return False

    # 2. 跑角色支线（如果指定）
    if route_file:
        f = os.path.join(SCRIPT_DIR, route_file)
        runner = ScriptRunner(f, player)
        result = runner.run(strategy)
        log_lines.append(f"  {route_file}: {len(runner.path_log):>3} 节点 → {result:<30} | {player.snapshot()}")
        if not result.startswith('END'):
            print('\n'.join(log_lines))
            print(f"  ❌ 在 {route_file} 出错：{result}")
            return False

    # 3. 推测应该走的结局并跑一遍
    max_aff = max(player.affinity.values())
    max_subj = max(player.affinity, key=player.affinity.get)
    if player.darkness >= 100:
        ending = 'ending_4_eternal_la.json'
    elif max_aff >= 80 and player.darkness < 60:
        ending = 'ending_2_best_love.json'  # 假设玩家选留下
    elif player.stress >= 90:
        ending = 'ending_5_hospital.json'
    else:
        ending = 'ending_1_good_grade.json'

    f = os.path.join(SCRIPT_DIR, ending)
    runner = ScriptRunner(f, player)
    result = runner.run(strategy)
    log_lines.append(f"  {ending}: {len(runner.path_log):>3} 节点 → {result:<30} | {player.snapshot()}")
    log_lines.append(f"  → 最高好感: {max_subj} ({max_aff}) | 黑化值: {player.darkness} | 压力: {player.stress}")
    log_lines.append(f"  → 触发结局: {ending}")

    print('\n'.join(log_lines))
    if not result.startswith('END'):
        print(f"  ❌ 在 {ending} 出错：{result}")
        return False

    print(f"  ✅ 路径完整跑通")
    return True

if __name__ == '__main__':
    ok_count = 0
    total = 0

    paths = [
        ('程设主攻线',     make_max_subject_strategy('ProgDesign'),     'route_progdesign.json'),
        ('高数主攻线',     make_max_subject_strategy('Calculus'),       'route_calculus.json'),
        ('线代主攻线（含黑化）', make_max_subject_strategy('LinearAlgebra'), 'route_linearalgebra.json'),
        ('AI 引论主攻线',   make_max_subject_strategy('AIIntro'),        'route_ai.json'),
        ('平衡线（默认）', balanced_strategy,                           None),
    ]
    for label, strat, route in paths:
        total += 1
        if play_path(label, strat, route_file=route):
            ok_count += 1

    print(f"\n{'='*78}")
    print(f"  汇总: {ok_count}/{total} 路径跑通")
    print(f"{'='*78}")
    sys.exit(0 if ok_count == total else 1)
