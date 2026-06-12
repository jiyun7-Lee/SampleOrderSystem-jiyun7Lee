#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <memory>
#include <limits>

#ifdef _DEBUG
#include <gtest/gtest.h>
#else
#include "model/repository/InMemorySampleRepository.h"
#include "model/repository/InMemoryOrderRepository.h"
#include "model/repository/InMemoryInventoryRepository.h"
#include "model/repository/InMemoryProductionRepository.h"
#include "service/SampleService.h"
#include "service/InventoryService.h"
#include "service/OrderService.h"
#include "controller/SampleController.h"
#include "controller/OrderController.h"
#include "view/SampleView.h"
#include "view/OrderView.h"
#include "util/SystemTimeProvider.h"
#endif

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

#ifdef _DEBUG
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#else
    std::cout << "=== S-Semi 반도체 시료 생산주문관리 시스템 ===" << std::endl;

    // repositories
    auto sampleRepo     = std::make_shared<InMemorySampleRepository>();
    auto orderRepo      = std::make_shared<InMemoryOrderRepository>();
    auto inventoryRepo  = std::make_shared<InMemoryInventoryRepository>();
    auto productionRepo = std::make_shared<InMemoryProductionRepository>();
    auto timeProvider   = std::make_shared<SystemTimeProvider>();

    // services
    auto sampleService    = std::make_shared<SampleService>(sampleRepo);
    auto inventoryService = std::make_shared<InventoryService>(*inventoryRepo);
    auto orderService     = std::make_shared<OrderService>(
        *orderRepo, *sampleRepo, *inventoryService, *productionRepo, *timeProvider);

    // views
    auto sampleView = std::make_shared<SampleView>();
    auto orderView  = std::make_shared<OrderView>();

    // controllers
    SampleController sampleController(sampleService, sampleView);
    auto orderController = std::make_shared<OrderController>(orderService, orderView);

    int choice = 0;
    while (true) {
        std::cout << "\n[1] 시료 관리\n[2] 시료 주문\n[3] 주문 승인/거절\n[0] 종료\n선택: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (choice == 1) {
            sampleController.Run();
        } else if (choice == 2) {
            orderController->PlaceOrder();
        } else if (choice == 3) {
            orderController->ProcessApproval();
        } else if (choice == 0) {
            break;
        }
    }

    return 0;
#endif
}
