#include "Subject.h"
#include <QtGlobal>

namespace SA {

void Subject::changeAffinity(int delta) {
    int newVal = qBound(AFFINITY_MIN, affinity_ + delta, AFFINITY_MAX);
    if (newVal != affinity_) {
        affinity_ = newVal;
        emit affinityChanged(affinity_);
    }
}

} // namespace SA
