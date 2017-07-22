#include "sniffer.h"

void ethernet(u_char *args, const struct pcap_pkthdr* pkthdr, const u_char* packet)
{
	struct ether_header *eth;
	int size = pkthdr->len;
	eth = (struct ether_header *)packet;
	int offset = sizeof(struct ether_header);


	printf("\nEthernet Header: \n\n");
	printf("\tDestination address : %s\n", ether_ntoa(eth->ether_dhost));
	printf("\tSource address : %s\n", ether_ntoa(eth->ether_shost));
	printf("\tProtocol: %.4X\n", ntohs(eth->ether_type));
       	printf("\tSize header : %d\n", size);
	printf("\tSize data payload : %d\n\n", size - offset);
	printData(packet + offset, size - offset);

}

void printData(const u_char* packet, int size)
{
	int i, j;
	int offset = 0;
	int nlines = size / 16;
	if(nlines * 16 < size)
		nlines++;
	printf("        ");
	for(i = 0; i < 16; i++)
		printf("%02X ", i);
	printf("\n\n");
	for(i = 0; i < nlines; i++){
		printf("        ");
		for(j = 0; j < 16; j++){
			if(offset + j >= size)
				printf("   ");
			else
				printf("%02X ", packet[offset + j]);
		}
		printf("    ");
		for(j = 0; j < 16; j++){
			if(offset + j >= size)
				printf("  ");
			else if(packet[offset + j] > 31 && packet[offset + j] < 127)
				printf("%c", packet[offset + j]);
			else
				printf(".");
		}
		offset += 16;
		printf("\n");
	}
}

int main()
{
	pcap_t *handle;
	char *device, errbuf[MAXSIZE];
	struct in_addr addr;
	bpf_u_int32 net, mask;

	device = pcap_lookupdev(errbuf);

	printf("Device: %s\n", device);

	if(pcap_lookupnet(device, &net, &mask, errbuf) < 0){
		perror("Pcap_lookupnet");
		exit(1);
	}
	addr.s_addr = net;
	printf("Net: %s\t", inet_ntoa(addr));
	addr.s_addr = mask;
	printf("Mask: %s\n", inet_ntoa(addr));

	handle = pcap_open_live(device, 65535, 1, 0, errbuf);
	pcap_loop(handle, -1, ethernet, NULL);

	pcap_close(handle);

	return 0;
}
