#include <iostream>
#include <windows.h>
#include <vector>
#include <ctime>
#include <stdexcept>
#include <mutex>
#include <algorithm>
#include "markerData.h"

std::mutex consoleMutex;

DWORD WINAPI MarkerThread(LPVOID param) {
    markerData* data = static_cast<markerData*>(param);
    srand(data->getId());

    WaitForSingleObject(data->getStartEvent(), INFINITE);

    std::vector<int>& array = data->getArray();
    int size = static_cast<int>(array.size());

    while (true) {
        int index = rand() % size;   // случайный индекс от 0 до size-1
        if (array[index] == 0) {    // если ячейка свободна
            Sleep(5);
            array[index] = data->getId();
            data->addMarkedIndex(index);
            Sleep(5);
        }
        else {   // если ячейка занята другим потоком
            {
                std::lock_guard<std::mutex> lock(consoleMutex);
                std::cout << "Поток #" << data->getId()
                    << " не может пометить элемент по индексу " << index
                    << ". Помечено: " << data->getMarkedIndexes().size() << std::endl;
            }

            SetEvent(data->getPauseEvent());    // пауза + сигнал main потоку что застрял
            WaitForSingleObject(data->getResumeEvent(), INFINITE);   // ожидаем указаний что делать
                                                                       // потом когда все потоки застрянут, один из них будет завершен, остальным дана команда продолжать
            if (data->shouldTerminate()) {
                break;
            }

            ResetEvent(data->getResumeEvent());
        }
    }

    data->clearMarks();
    return 0;
}

void printArray(const std::vector<int>& array) {
    std::lock_guard<std::mutex> lock(consoleMutex);
    std::cout << "Массив: ";
    for (int val : array) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

int main() {
    setlocale(LC_ALL, "Russian");

    try {
        int arraySize;
        std::cout << "Введите размер массива: ";
        if (!(std::cin >> arraySize) || arraySize <= 0) {
            throw std::invalid_argument("Размер массива должен быть положительным числом.");
        }

        std::vector<int> array(arraySize, 0);

        int numThreads;
        std::cout << "Введите количество потоков: ";
        if (!(std::cin >> numThreads) || numThreads <= 0) {
            throw std::invalid_argument("Количество потоков должно быть положительным числом.");
        }

        std::vector<markerData*> markers;

        for (int i = 1; i <= numThreads; ++i) {
            markerData* data = new markerData(i, array);
            HANDLE hThread = CreateThread(NULL, 0, MarkerThread, data, 0, NULL);
            data->setThreadHandle(hThread);
            markers.push_back(data);
        }

        for (auto& m : markers) {
            SetEvent(m->getStartEvent());
        }

        while (!markers.empty()) {
            for (auto& m : markers) {
                WaitForSingleObject(m->getPauseEvent(), INFINITE);
            }

            printArray(array);

            {
                std::lock_guard<std::mutex> lock(consoleMutex);
                std::cout << "Активные потоки: ";
                for (auto& m : markers) {
                    std::cout << m->getId() << " ";
                }
                std::cout << std::endl;
            }

            int idToStop;
            std::cout << "Введите номер потока, который нужно завершить: ";
            if (!(std::cin >> idToStop)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cerr << "Ошибка: нужно ввести корректное число.\n";
                continue;
            }

            markerData* toStop = nullptr;
            for (auto it = markers.begin(); it != markers.end(); ++it) {
                if ((*it)->getId() == idToStop) {
                    toStop = *it;
                    markers.erase(it);
                    break;
                }
            }

            if (!toStop) {
                std::cerr << "Ошибка: потока с таким номером не существует или он уже завершён.\n";
                continue;
            }

            toStop->setTerminate(true);
            SetEvent(toStop->getResumeEvent());
            WaitForSingleObject(toStop->getThreadHandle(), INFINITE);
            CloseHandle(toStop->getThreadHandle());
            delete toStop;

            printArray(array);

            for (auto& m : markers) {
                ResetEvent(m->getPauseEvent());
                SetEvent(m->getResumeEvent());
            }
        }

        bool allZero = std::all_of(array.begin(), array.end(), [](int x) { 
                return x == 0; 
            });
        std::cout << (allZero ? "Массив успешно очищен." : "Массив не полностью очищен.") << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
