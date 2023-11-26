
#include "SaveProtFrame.h"

#define _CRT_SECURE_NO_WARNINGS

HINSTANCE ModuleI;

ProtocolSaveProc* ProtS;


int InitSaveProtData(HINSTANCE MInst, ProtocolSaveProc* PS)
{
	ModuleI = MInst;
	ProtS = PS;
	return 0;
}



/*******************************************************************************************************
*******************************************************************************************************




/////////////////////          F U N K C J E        Z A P I S U        P R O T K O £ Ó W




*******************************************************************************************************
*****************************************************************************************************/




int SaveTCP(PVOID Frame, fstream* pl, unsigned short TcpSize)
{
	TcpProt* TcpFrame;
	unsigned short port_zr, port_doc;
	unsigned long nr_sekw;
	char Text[500];
	char Text1[500];
	char str[45];
	TcpFlags TcpF;
	int i, licz, index;
	int rozm;
	unsigned char* bt;
	DataOffset DOffs;


	if (TcpSize <= 0)
		return 1;

	TcpFrame = (TcpProt*)Frame;

	MakeShortNumber((unsigned short*)TcpFrame->PortZrodl, &port_zr);
	MakeShortNumber((unsigned short*)TcpFrame->PortDocel, &port_doc);
	MakeLONGNumber((unsigned long*)TcpFrame->NumerSekw, &nr_sekw);

	memset(Text, 0, 500);
	memset(str, 0, 45);

	_ui64toa_s(nr_sekw, str, 45, 10);

	sprintf_s(Text, "**********RAMKA : TCP**********\n\nPORT RÓD£OWY : %d  PORT DOCELOWY : %d\n\nNUMER SEKWENCJI : %s\n\n", port_zr, port_doc, str);

	pl->write(Text, strlen(Text));

	MakeTcpFlags(TcpFrame->Flagi, &TcpF);

	memset(Text, 0, 500);

	sprintf(Text, "FLAGI : CWR-%d ECE-%d URG-%d ACK-%d PSH-%d RST-%d SYN-%d FIN-%d\n\n**********DANE**********\n\n", TcpF.CWR, TcpF.ECE, TcpF.URG, TcpF.ACK, TcpF.PSH, \
		TcpF.RST, TcpF.SYN, TcpF.FIN);

	pl->write(Text, strlen(Text));

	MakeTcp_DataOffset(TcpFrame->DlugNagl_Reserved, &DOffs);

	if (DOffs.DataOffs == 5)
	{
		bt = (unsigned char*)TcpFrame->Dane;

	}
	else
	{
		bt = (unsigned char*)TcpFrame;
		bt = bt + (4 * (int)DOffs.DataOffs);
	}

	rozm = TcpSize - (4 * DOffs.DataOffs);

	if (rozm <= 0)
		return 2;

	licz = 0;

	memset(Text1, 0, 500);
	memset(Text, 0, 500);

	for (i = 0; i < rozm; i++)
	{
		if(bt[i]<=32 || bt[i]>127)
	//	if ((bt[i] >= 0 && bt[i] <= 32) || (bt[i]==0xAD) || (bt[i]==0xA0))
			sprintf(&Text[strlen(Text)], ".");
		else
			sprintf(&Text[strlen(Text)], "%c", bt[i]);

		sprintf(&Text1[strlen(Text1)], "%0.2X ", bt[i]);

		licz++;

		if (licz == 20)
		{
			sprintf(&Text1[strlen(Text1)], "\n");
			sprintf(&Text[strlen(Text)], "                    ");

			pl->write(Text, strlen(Text));

			pl->write(Text1, strlen(Text1));

			memset(Text1, 0, 500);
			memset(Text, 0, 500);

			licz = 0;
		}
	}

	if (licz > 0)
	{
		sprintf(&Text1[strlen(Text1)], "\n");
		sprintf(&Text[strlen(Text)], "                    ");

		for (i = licz; i < 20; i++)
			sprintf(&Text[strlen(Text)], " ");

		pl->write(Text, strlen(Text));

		pl->write(Text1, strlen(Text1));
	}

	return 0;
}

int SaveUDP(PVOID Frame, fstream* pl, unsigned short UdpSize)
{
	UDPProt* UDPFrame;
	unsigned short port_zr, port_doc;
	char Text[500];
	char Text1[500];
	int i, licz;
	int rozm;
	unsigned char* bt;

	if (UdpSize <= 0)
		return 1;

	UDPFrame = (UDPProt*)Frame;

	MakeShortNumber((unsigned short*)UDPFrame->port_zrodl, &port_zr);
	MakeShortNumber((unsigned short*)UDPFrame->port_docel, &port_doc);

	memset(Text, 0, 500);


	sprintf_s(Text, "**********RAMKA : UDP**********\n\nPORT RÓD£OWY : %d  PORT DOCELOWY : %d\n\n**********DANE**********\n\n", port_zr, port_doc);

	pl->write(Text, strlen(Text));




	bt = (unsigned char*)UDPFrame;
	bt = bt + 8;


	rozm = UdpSize - 8;

	if (rozm <= 0)
		return 2;

	licz = 0;

	memset(Text1, 0, 500);
	memset(Text, 0, 500);

	for (i = 0; i < rozm; i++)
	{

		if (bt[i] <= 32 || bt[i] > 127)
	//	if ((bt[i] >= 0 && bt[i] <= 32) || (bt[i]==0xAD) || (bt[i]==0xA0))
			sprintf(&Text[strlen(Text)], ".");
		else
			sprintf(&Text[strlen(Text)], "%c", bt[i]);

		sprintf(&Text1[strlen(Text1)], "%0.2X ", bt[i]);

		licz++;

		if (licz == 20)
		{
			sprintf(&Text1[strlen(Text1)], "\n");
			sprintf(&Text[strlen(Text)], "			");

			pl->write(Text, strlen(Text));

			pl->write(Text1, strlen(Text1));

			memset(Text1, 0, 500);
			memset(Text, 0, 500);

			licz = 0;
		}
	}

	if (licz > 0)
	{
		sprintf(&Text1[strlen(Text1)], "\n");
		sprintf(&Text[strlen(Text)], "                    ");

		for (i = licz; i < 20; i++)
			sprintf(&Text[strlen(Text)], " ");

		pl->write(Text, strlen(Text));

		pl->write(Text1, strlen(Text1));
	}

	return 0;
}


int SaveHopByHop(PVOID Frame, fstream* pl, unsigned short DataSize)
{
	HbyH* HbHFrame;
	JumboPacket* Jumbo;
	PVOID P;
	unsigned short rozmiar;
	unsigned long JumboRozm;
	int i;

	HbHFrame = (HbyH*)Frame;


	if (DataSize == 0)
	{
		Jumbo = (JumboPacket*)HbHFrame->Dane;

		MakeLONGNumber((unsigned long*)Jumbo->JumboDataLen, &JumboRozm);

		if (JumboRozm < 8)
			return 1;

		rozmiar = JumboRozm - 8;

	}
	else
		rozmiar = DataSize - 8;

	if (HbHFrame->DlugoscRozszerz > 0)
	{
		rozmiar = rozmiar - HbHFrame->DlugoscRozszerz;

		P = (PVOID)(((char*)Frame) + 8 + HbHFrame->DlugoscRozszerz);
	}
	else
		P = (PVOID)(((char*)Frame) + 8);



	if (rozmiar <= 0)
		return 1;

	for (i = 0; i < 146; i++)
	{
		if ((unsigned int)HbHFrame->NastNaglowek == i && ProtS[i].isDefined != 0 && ProtS[i].Proc != 0 && rozmiar > 0)
			(*ProtS[i].Proc)(P, pl, rozmiar);
	}

	return 0;
}