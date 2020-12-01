#include <iostream>

#include "FlashController.hpp"
#include "FlashDriver.hpp"
#include "SPI.hpp"
#include "mpsse/libMPSSE_spi.h"
#include <ftd2xx.h>

std::uint32_t listDevices()
{
    DWORD devicesCount = 0;

    FT_CreateDeviceInfoList(&devicesCount);
    auto devices = new FT_DEVICE_LIST_INFO_NODE[devicesCount];

    auto r = FT_GetDeviceInfoList(devices, &devicesCount);
    std::cout << "Devices count: " << devicesCount << std::endl;

    for(auto i = 0; i < devicesCount; i++)
    {
        std::cout << devices[i].Description << "\t" << devices[i].SerialNumber << std::endl;
    }

    return devicesCount;
}

std::uint32_t findChannelByPort(const char port)
{
    std::uint32_t channels;
    SPI_GetNumChannels(&channels);
    printf("\nAvailable SPI channels count: %d\n", channels);
    for(auto i = 0; i < channels; i++)
    {
        FT_DEVICE_LIST_INFO_NODE info;
        SPI_GetChannelInfo(i, &info);

        //        std::cout << i << ": " << devices[i].Description << "\t" << devices[i].SerialNumber << std::endl;

        printf("\n\nDev %d:\n", i);
        printf("  Flags=0x%x\n", info.Flags);
        printf(" Type=0x%x\n", info.Type);
        printf("  ID=0x%x\n", info.ID);
        printf(" LocId=0x%x\n", info.LocId);
        printf("  SerialNumber=%s\n", info.SerialNumber);
        printf(" Description=%s\n", info.Description);
        printf("  ftHandle=0x%x\n", info.ftHandle);

        std::size_t length = strlen(info.Description);
        if(info.Description[length - 1] == port)
        {
            return i;
        }
    }

    return -1;
}

std::uint32_t listChannels()
{
    std::uint32_t channels;
    SPI_GetNumChannels(&channels);
    printf("\nAvailable SPI channels count: %d\n", channels);
    for(auto i = 0; i < channels; i++)
    {
        FT_DEVICE_LIST_INFO_NODE info;
        SPI_GetChannelInfo(i, &info);

        std::cout << i << ": " << info.Description << "\t" << info.SerialNumber << std::endl;
    }
    return channels;
}

int main(int argc, char** argv)
{
    Init_libMPSSE();

    if(listDevices() > 0)
    {
        uint32_t channels = listChannels();

        uint32_t channelNo = -1;

        std::cout << "\nChoose channel: ";
        std::cin >> channelNo;

        if(channelNo < channels)
        {
            std::cout << "\nUsing channel: " << channelNo << "\n" << std::endl;
            SPI spi(channelNo, 1 * 1000 * 1000, ChipSelectLine3 | ChipSelectActiveLow | SpiMode0);

            spi.ChipSelect(false);

            flash::FlashDriver flash{spi};

            flash::FlashController flashController{flash};

            flashController.ReadId();
            flashController.ReadStatus();
        }
        else
        {
            std::cout << "\nWrong channel: " << channelNo << ". It has to be smaller than "
                      << channels << "." << std::endl;
        }

        //        if(argc == 2 && strcmp(argv[1], "id") == 0)
        //        {
        //            flashController.ReadId();
        //            return 0;
        //        }
        //
        //        if(argc == 3 && strcmp(argv[1], "read") == 0)
        //        {
        //            std::cout << "Reading memory to " << argv[2] << std::endl;
        //            flashController.ReadAllMemory(argv[2]);
        //            return 0;
        //        }
        //
        //        if(argc == 2 && strcmp(argv[1], "erase_special") == 0)
        //        {
        //            std::cout << "Erasing spacial " << std::endl;
        //            flashController.EraseRange(0x2000, 0x5FE000);
        //            return 0;
        //        }
        //
        //        if(argc == 2 && strcmp(argv[1], "erase_normal") == 0)
        //        {
        //            std::cout << "Erasing normal " << std::endl;
        //            flashController.EraseRange(0x600000, 0x7FC000);
        //            return 0;
        //        }
        //        if(argc == 2 && strcmp(argv[1], "read") == 0)
        //        {
        //            std::cout << "Reading " << std::endl;
        //            flashController.ReadAllMemory("alicja.txt");
        //            return 0;
        //        }
        //        if(argc == 2 && strcmp(argv[1], "write") == 0)
        //        {
        //            std::cout << "Writing " << std::endl;
        //            flashController.WriteSomething();
        //            return 0;
        //        }
    }

    std::cout << "\nPress any key to continue...";
    std::getchar();
    std::getchar();
    return 0;
}