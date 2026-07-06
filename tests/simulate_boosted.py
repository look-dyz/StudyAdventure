#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
高数值场景验证：模拟"小游戏奖励"把玩家好感度提到 50+ 与 80+，
确认信物节点、route lv2/endgame 分流、Best Love 结局、永远的线代结局都能跑通。
"""

import os, sys
sys.path.insert(0, os.path.dirname(__file__))
from simulate_routes import Player, ScriptRunner, make_max_subject_strategy, SCRIPT_DIR

def boosted(target, boost_affinity=0, boost_darkness=0):
    p = Player()
    if target:
        p.affinity[target] = boost_affinity
    p.darkness = boost_darkness
    return p

def trace_route_hub(label, target_subject, affinity_value):
    """专测 route_X.json 的 hub 分流是否把不同好感度的玩家送到对的节点"""
    p = boosted(target_subject, affinity_value)
    f_map = {
        'ProgDesign': 'route_progdesign.json',
        'Calculus': 'route_calculus.json',
        'AIIntro': 'route_ai.json',
    }
    fname = f_map[target_subject]
    runner = ScriptRunner(os.path.join(SCRIPT_DIR, fname), p)
    result = runner.run(make_max_subject_strategy(target_subject))
    first_after_hub = runner.path_log[1] if len(runner.path_log) > 1 else 'N/A'
    print(f"  [{label:<22}] {target_subject:<13} affinity={affinity_value:>3} → hub 分流到 {first_after_hub:<28} | {result}")
    return result.startswith('END'), first_after_hub

def trace_la_hub(label, darkness_value):
    p = boosted('LinearAlgebra', 30)
    p.darkness = darkness_value
    runner = ScriptRunner(os.path.join(SCRIPT_DIR, 'route_linearalgebra.json'), p)
    result = runner.run(make_max_subject_strategy('LinearAlgebra'))
    first_after_hub = runner.path_log[1] if len(runner.path_log) > 1 else 'N/A'
    print(f"  [{label:<22}] LinearAlgebra darkness={darkness_value:>3} → hub 分流到 {first_after_hub:<28} | {result}")
    return result.startswith('END'), first_after_hub

# ============ route hub 分流测试 ============
print(f"\n{'='*78}\n  Hub 分流验证：每条线在 lv1/lv2/endgame 三档好感度下走对节点\n{'='*78}")
hub_cases = [
    # (label, subject, affinity, expected_first_node)
    ('PD lv1',    'ProgDesign', 20, 'progdesign_lv1'),
    ('PD lv2',    'ProgDesign', 70, 'progdesign_lv2'),
    ('CA lv1',    'Calculus',   20, 'calculus_lv1'),
    ('CA lv2',    'Calculus',   60, 'calculus_lv2_phil'),
    ('CA endgame','Calculus',   90, 'calculus_endgame'),
    ('AI lv1',    'AIIntro',    20, 'ai_lv1'),
    ('AI lv2',    'AIIntro',    60, 'ai_lv2_chat'),
    ('AI endgame','AIIntro',    90, 'ai_endgame'),
]
hub_ok = 0
for label, subj, aff, expected in hub_cases:
    ok, actual = trace_route_hub(label, subj, aff)
    status = '✓' if actual == expected else f'✗ 期望 {expected}'
    if ok and actual == expected: hub_ok += 1
    print(f"    分流结果: {status}")

print(f"\n  线代 hub 按 darkness 分流：")
la_cases = [
    ('LA lv1',    10, 'la_lv1'),
    ('LA lv2',    50, 'la_lv2_dark'),
    ('LA endgame',80, 'la_endgame'),
]
la_ok = 0
for label, dk, expected in la_cases:
    ok, actual = trace_la_hub(label, dk)
    status = '✓' if actual == expected else f'✗ 期望 {expected}'
    if ok and actual == expected: la_ok += 1
    print(f"    分流结果: {status}")

# ============ 信物节点触发测试 ============
print(f"\n{'='*78}\n  信物节点解锁验证：affinity >= 50 才能在 week4 拿到信物\n{'='*78}")

def test_gift_unlock(subject_name, affinity_value, expect_visible):
    p = boosted(subject_name, affinity_value)
    p.week = 4
    f = os.path.join(SCRIPT_DIR, 'week4.json')
    import json
    doc = json.load(open(f))
    gift_node = next(n for n in doc['nodes'] if n['id'] == 'w4_check_gifts')
    from simulate_routes import eval_cond
    visible_count = 0
    for c in gift_node['choices']:
        cond = c.get('condition', '')
        if not cond or eval_cond(cond, p):
            visible_count += 1
    has_gift_choice = visible_count > 1  # 至少有"找谁"+"独自"两个
    pass_ = has_gift_choice == expect_visible
    print(f"  {subject_name:<14} affinity={affinity_value:>3} | 可见选项 {visible_count} 个 | "
          f"{'解锁信物 ✓' if has_gift_choice else '只能独自 ✓'} | {'PASS' if pass_ else 'FAIL'}")
    return pass_

gift_ok = 0
gift_tests = [
    ('ProgDesign',    49, False),
    ('ProgDesign',    50, True),
    ('Calculus',      50, True),
    ('LinearAlgebra', 50, True),
    ('AIIntro',       50, True),
]
for subj, aff, exp in gift_tests:
    if test_gift_unlock(subj, aff, exp): gift_ok += 1

# ============ week5 黑化结局触发测试 ============
print(f"\n{'='*78}\n  week5 最终选择黑化结局触发：darkness >= 60 才能/会被锁定\n{'='*78}")

def test_w5_final_choice(darkness, expect_eternal_visible):
    p = boosted(None)
    p.darkness = darkness
    import json
    f = os.path.join(SCRIPT_DIR, 'week5.json')
    doc = json.load(open(f))
    node = next(n for n in doc['nodes'] if n['id'] == 'w5_final_choice')
    from simulate_routes import eval_cond
    seen_texts = []
    for c in node['choices']:
        cond = c.get('condition', '')
        if not cond or eval_cond(cond, p):
            seen_texts.append(c['text'])
    # 黑化结局特征：选项文本包含"脚步动不了"或"不允许你走"
    eternal_visible = any('脚步动不了' in t or '不允许你走' in t for t in seen_texts)
    pass_ = eternal_visible == expect_eternal_visible
    print(f"  darkness={darkness:>3} | 可选文本: {seen_texts} | "
          f"黑化结局{'解锁' if eternal_visible else '锁住'} | {'PASS' if pass_ else 'FAIL'}")
    return pass_

w5_ok = 0
for dk, exp in [(30, False), (59, False), (60, True), (90, True)]:
    if test_w5_final_choice(dk, exp): w5_ok += 1

# ============ 5 个结局文件全部能跑通到 END ============
print(f"\n{'='*78}\n  5 个结局剧本独立跑通验证\n{'='*78}")
ending_ok = 0
for ending in ['ending_1_good_grade.json', 'ending_2_best_love.json',
               'ending_3_farewell.json', 'ending_4_eternal_la.json',
               'ending_5_hospital.json']:
    p = boosted(None)
    runner = ScriptRunner(os.path.join(SCRIPT_DIR, ending), p)
    result = runner.run(make_max_subject_strategy('ProgDesign'))
    print(f"  {ending:<35} | {len(runner.path_log):>3} 节点 → {result}")
    if result.startswith('END'): ending_ok += 1

# ============ 汇总 ============
total = len(hub_cases) + len(la_cases) + len(gift_tests) + 4 + 5
passed = hub_ok + la_ok + gift_ok + w5_ok + ending_ok
print(f"\n{'='*78}\n  汇总: {passed}/{total} 通过\n{'='*78}")
sys.exit(0 if passed == total else 1)
