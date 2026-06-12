#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <memory>
#include <limits>

#ifdef _DEBUG
#include <gtest/gtest.h>
#else
#include "persistence/SampleJsonRepository.h"
#include "persistence/OrderJsonRepository.h"
#include "persistence/InventoryJsonRepository.h"
#include "persistence/ProductionJobJsonRepository.h"
#include "service/SampleService.h"
#include "service/InventoryService.h"
#include "service/OrderService.h"
#include "service/ProductionService.h"
#include "service/ReleaseService.h"
#include "service/MonitoringService.h"
#include "controller/SampleController.h"
#include "controller/OrderController.h"
#include "controller/ProductionController.h"
#include "controller/ReleaseController.h"
#include "controller/MonitoringController.h"
#include "view/SampleView.h"
#include "view/OrderView.h"
#include "view/ProductionView.h"
#include "view/ReleaseView.h"
#include "view/MonitoringView.h"
#include "util/SystemTimeProvider.h"
#include <filesystem>
#endif

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

#ifdef _DEBUG
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#else
    std::filesystem::create_directories("data");

    // repositories (JSON 영속성)
    auto sampleRepo     = std::make_shared<SampleJsonRepository>("data/samples.json");
    auto orderRepo      = std::make_shared<OrderJsonRepository>("data/orders.json");
    auto inventoryRepo  = std::make_shared<InventoryJsonRepository>("data/inventory.json");
    auto productionRepo = std::make_shared<ProductionJobJsonRepository>("data/production_queue.json");
    auto timeProvider   = std::make_shared<SystemTimeProvider>();

    // services
    auto sampleService      = std::make_shared<SampleService>(sampleRepo);
    auto inventoryService   = std::make_shared<InventoryService>(*inventoryRepo);
    auto orderService       = std::make_shared<OrderService>(
        *orderRepo, *sampleRepo, *inventoryService, *productionRepo, *timeProvider);
    auto productionService  = std::make_shared<ProductionService>(
        *productionRepo, *inventoryRepo, *orderRepo, *timeProvider);
    auto releaseService     = std::make_shared<ReleaseService>(*orderRepo, *inventoryService);
    auto monitoringService  = std::make_shared<MonitoringService>(
        *orderRepo, *inventoryRepo, *sampleRepo);

    // views
    auto sampleView     = std::make_shared<SampleView>();
    auto orderView      = std::make_shared<OrderView>();
    auto productionView = std::make_shared<ProductionView>();
    auto releaseView    = std::make_shared<ReleaseView>();
    auto monitoringView = std::make_shared<MonitoringView>();

    // controllers
    SampleController     sampleController(sampleService, sampleView);
    auto orderController      = std::make_shared<OrderController>(orderService, orderView);
    auto productionController = std::make_shared<ProductionController>(productionService, productionView);
    auto releaseController    = std::make_shared<ReleaseController>(releaseService, releaseView);
    auto monitoringController = std::make_shared<MonitoringController>(monitoringService, monitoringView);

    std::cout << "=== S-Semi 반도체 시료 생산주문관리 시스템 ===" << std::endl;

    int choice = 0;
    while (true) {
        std::cout << "\n[1] 시료 관리\n[2] 시료 주문\n[3] 주문 승인/거절\n"
                     "[4] 모니터링\n[5] 생산라인 조회\n[6] 출고 처리\n[0] 종료\n선택: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            sampleController.Run();
        } else if (choice == 2) {
            orderController->PlaceOrder();
        } else if (choice == 3) {
            orderController->ProcessApproval();
        } else if (choice == 4) {
            monitoringController->HandleMonitoring();
        } else if (choice == 5) {
            productionController->ShowProductionLine();
        } else if (choice == 6) {
            releaseController->HandleRelease();
        } else if (choice == 0) {
            break;
        }
    }

    return 0;
#endif
}
