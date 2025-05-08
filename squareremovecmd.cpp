#include "squareremovecmd.h"

#include <QSet>

SquareRemoveCmd::SquareRemoveCmd(FortuneAvenueGraphicsScene *scene, const std::function<void (const QVector<SquareData> &, bool)> &updateFn) : scene(scene), updateFn(updateFn)
{
    auto sel = scene->selectedItems();
    for (auto &elem: sel) {
        removedItems.push_back(((SquareItem *)elem)->getData());
    }
    std::sort(removedItems.begin(), removedItems.end(), [](const SquareData &A, const SquareData &B) {
        return A.id < B.id;
    });
    setText(FortuneAvenueGraphicsScene::tr("Remove Squares"));
}

void SquareRemoveCmd::undo()
{
    auto items = scene->squareItems();
    for (auto &elem: items) {
        elem->getData().id = newToOldIDs[elem->getData().id];
        elem->getData().waypoints = oldIDToWaypoints[elem->getData().id];
        elem->updateZValueFromData();
    }
    for (auto &elem: removedItems) {
        scene->addItem(new SquareItem(elem));
    }
    scene->update();
    updateFn(removedItems, false);
}

void SquareRemoveCmd::redo()
{
    QSet<int> indicesToRemove;
    for (auto &elem: removedItems) {
        indicesToRemove.insert(elem.id);
    }
    auto items = scene->squareItems();
    for (auto &elem: items) {
        if (indicesToRemove.contains(elem->getData().id)) {
            scene->removeItem(elem);
            delete elem;
        }
    }

    items = scene->squareItems();
    // fix square ids
    oldToNewIDs.clear();
    for (int i=0; i<items.size(); ++i) {
        oldToNewIDs[items[i]->getData().id] = i;
        oldIDToWaypoints[items[i]->getData().id] = items[i]->getData().waypoints;
        newToOldIDs[i] = items[i]->getData().id;
        items[i]->getData().id = i;
        items[i]->updateZValueFromData();
    }

    // and waypoints
    for (auto item: std::as_const(items)) {
        for (auto &waypoint: item->getData().waypoints) {
            waypoint.entryId = oldToNewIDs.value(waypoint.entryId, 255);
            for (auto &dest: waypoint.destinations) {
                dest = oldToNewIDs.value(dest, 255);
            }
        }
    }
    scene->update();
    updateFn(removedItems, true);
}
