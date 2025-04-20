#include "markerData.h"

markerData::markerData(int id, std::vector<int>& array)
    : id(id), array(array), terminate(false)
{
    startEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    pauseEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    resumeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    threadHandle = NULL;
}

void markerData::addMarkedIndex(int index) {
    markedIndexes.push_back(index);
}

void markerData::clearMarks() {
    for (int idx : markedIndexes) {
        array[idx] = 0;
    }
    markedIndexes.clear();
}

int markerData::getId() const {
    return id;
}

HANDLE markerData::getStartEvent() const {
    return startEvent;
}

HANDLE markerData::getPauseEvent() const {
    return pauseEvent;
}

HANDLE markerData::getResumeEvent() const {
    return resumeEvent;
}

HANDLE markerData::getThreadHandle() const {
    return threadHandle;
}

void markerData::setThreadHandle(HANDLE handle) {
    threadHandle = handle;
}

std::vector<int>& markerData::getArray() {
    return array;
}

std::vector<int>& markerData::getMarkedIndexes() {
    return markedIndexes;
}

void markerData::setTerminate(bool value) {
    terminate = value;
}

bool markerData::shouldTerminate() const {
    return terminate;
}
