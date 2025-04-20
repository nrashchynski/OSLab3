#ifndef MARKERDATA_H
#define MARKERDATA_H

#include <windows.h>
#include <vector>

class markerData {
public:
    markerData(int id, std::vector<int>& array);

    void addMarkedIndex(int index);
    void clearMarks();

    int getId() const;
    HANDLE getStartEvent() const;
    HANDLE getPauseEvent() const;
    HANDLE getResumeEvent() const;
    HANDLE getThreadHandle() const;
    void setThreadHandle(HANDLE handle);
    std::vector<int>& getArray();
    std::vector<int>& getMarkedIndexes();

    void setTerminate(bool value);
    bool shouldTerminate() const;

private:
    int id;
    std::vector<int>& array;
    std::vector<int> markedIndexes;

    HANDLE startEvent;
    HANDLE pauseEvent;
    HANDLE resumeEvent;
    HANDLE threadHandle;

    bool terminate;
};

#endif // MARKERDATA_H
