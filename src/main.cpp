#include <windows.h>
#include <iostream>
#include <memory>
#include "model/repository/InMemorySampleRepository.h"
#include "service/SampleService.h"
#include "controller/SampleController.h"
#include "view/SampleView.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::cout << "=== S-Semi 반도체 시료 생산주문관리 시스템 ===" << std::endl;

    auto sampleRepo = std::make_shared<InMemorySampleRepository>();
    auto sampleService = std::make_shared<SampleService>(sampleRepo);
    auto sampleView = std::make_shared<SampleView>();
    SampleController sampleController(sampleService, sampleView);

    int choice = 0;
    while (true) {
        std::cout << "\n[1] 시료 관리\n[0] 종료\n선택: ";
        std::cin >> choice;
        if (choice == 1) {
            sampleController.Run();
        } else if (choice == 0) {
            break;
        }
    }

    return 0;
}
