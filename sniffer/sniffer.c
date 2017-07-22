#include "sniffer.h"

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *buf)
{
        int size = header->len;
        struct ethhdr *eth = (struct ethhdr *)buf;

        printf("\n");
        printf("Ethernet Header\n");
        printf("\tDestination address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n",
                                         eth->h_dest[0], eth->h_dest[1], eth->h_dest[2],
                                         eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
        printf("\tSource address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n",
                                         eth->h_source[0], eth->h_source[1], eth->h_source[2],
                                         eth->h_source[3], eth->h_source[4], eth->h_source[5]);
        printf("\tProtocol : %u \n", (unsigned short)eth->h_proto);
	printf("\tSize Header : %d\n", size);
	printf("\tSize data payload : %d\n", size - sizeof(struct ethhdr));
        printf("\n\tData Payload\n");
        printData(buf+sizeof(struct ethhdr), size-sizeof(struct ethhdr));
}

void printData(const u_char *data, int size)
{
        int i, j;

        for(i = 0; i < size; i++){
                if(i != 0 && i%16 == 0){
                        printf("      ");
                        for(j = i - 16; j < i; j++){
                                if(data[j] >= 32 && data[j] <= 128)
                                        printf("%c", (unsigned char)data[j]);
                                else
                                        printf(".");
                        }
                        printf("\n");
                }
                if(i%16 == 0)
                        printf("   ");
                if(i == size - 1){
                        for(j = 0; j < 15 - i%16; j++)
                                printf("   ");
                        printf("     ");
                        for(j = i - i%16; j <=i; j++){
                                if(data[j] >= 32 && data[j] <= 128)
                                        printf("%c", (unsigned char)data[j]);
				else
                                        printf(".");
                        }
                        printf("\n");
                }
        }
}


int main()
{
        pcap_if_t *alldev, *device;
        pcap_t *handle;
        char errbuf[MAXSIZE], devs[100][100];
        int count = 1, n;

	printf("Finding available devices...\n");
        if(pcap_findalldevs(&alldev, errbuf)){
                printf("Error finding devices: %s\n", errbuf);
                exit(1);
        }
        printf("Done\n");
        printf("Available devices are: \n");
        for(device = alldev; device != NULL; device = device->next){
                printf("%d. %s - %s\n", count, device->name, device->description);
                if(device->name != NULL){
                        strcpy(devs[count], device->name);
                }
                count++;
        }
        printf("Enter the number of the device you want to sniff: ");
        scanf("%d", &n);
        printf("Opening device %s for sniffing...\n", devs[n]);
        handle = pcap_open_live(devs[n], 65535, 1, 0, errbuf);
        if(handle == NULL){
                fprintf(stderr, "Couldn't open device %s : %s\n", devs[n], errbuf);
                exit(1);
        }
        printf("Done\n");
        pcap_loop(handle, -1, process_packet, NULL);

        return 0;
}


