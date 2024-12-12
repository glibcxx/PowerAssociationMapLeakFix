#include "PowerAssociationMapLeakFix.h"

#include <ll/api/memory/Hook.h>
#include <ll/api/memory/Memory.h>
#include <ll/api/mod/RegisterHelper.h>

#include <mc/world/redstone/circuit/CircuitSceneGraph.h>
#include <mc/world/redstone/circuit/components/BaseCircuitComponent.h>

namespace pa_map_fix {

static std::unique_ptr<PowerAssociationMapLeakFix> instance;

PowerAssociationMapLeakFix& PowerAssociationMapLeakFix::getInstance() { return *instance; }

bool PowerAssociationMapLeakFix::load() {
    getSelf().getLogger().debug("Loading...");
    // Code for loading the mod goes here.
    return true;
}

bool PowerAssociationMapLeakFix::enable() {
    getSelf().getLogger().debug("Enabling...");
    // Code for enabling the mod goes here.
    return true;
}

bool PowerAssociationMapLeakFix::disable() {
    getSelf().getLogger().debug("Disabling...");
    // Code for disabling the mod goes here.
    return true;
}


LL_AUTO_TYPE_INSTANCE_HOOK(
    RemoveStaleRelationshipsHook,
    ll::memory::HookPriority::Highest,
    CircuitSceneGraph,
    &CircuitSceneGraph::removeStaleRelationships,
    void
) {
    for (auto iterUpdate = this->mPendingUpdates.begin(); iterUpdate != this->mPendingUpdates.end(); iterUpdate++) {
        BlockPos& posUpdate            = iterUpdate->second.mPos;
        auto      powerAssociationIter = this->mPowerAssociationMap.find(posUpdate);
        if (powerAssociationIter != this->mPowerAssociationMap.end()) {
            CircuitComponentList& relationships = powerAssociationIter->second;
            for (auto perChunkIter = relationships.mComponents.begin(); perChunkIter != relationships.mComponents.end();
                 perChunkIter      = relationships.mComponents.erase(perChunkIter)) {
                auto allListIter = this->mAllComponents.find(perChunkIter->mPos);
                if (allListIter != mAllComponents.end()) {
                    allListIter->second->removeSource(posUpdate, iterUpdate->second.mRawComponentPtr);
                }
            }
        }
    }
}

} // namespace pa_map_fix

LL_REGISTER_MOD(pa_map_fix::PowerAssociationMapLeakFix, pa_map_fix::instance);
