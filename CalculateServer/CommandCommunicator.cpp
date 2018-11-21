#include "CommandCommunicator.h"
CommandCommunicator* CommandCommunicator::m_pInstance=nullptr;

void CommandCommunicatorMain(SOCKET client)
{
	CommandCommunicator* cmdCommunicator = CommandCommunicator::GetInstance();
	while (true) {
		CommandMaker* cmdMaker = cmdCommunicator->commandMaker;
		cv::Mat img = cmdCommunicator->recver.recvIMG();
		cv::imshow("recv_test", img);
		cv::waitKey(1);
		if (cmdMaker != nullptr) {
			
			Command cmd = cmdMaker->makeCommand(img);
			const int cmdLen = cmdMaker->getCommandLen();
			int sendLen = 0;
			char lenBuffer[4];

			memcpy(lenBuffer, (void*)&cmdLen, sizeof(int));
			send(client, lenBuffer, 4, 0);
			char *buffer = new char[cmdLen];
			memcpy(buffer, &cmd, sizeof(Command));
			sendLen = 0;
			while (sendLen < cmdLen) {
				sendLen += send(client, buffer + sendLen, cmdLen, 0);
			}
			
			memset(buffer, 0, sendLen);
			int recvLen = 0;
			while (recvLen < 4) {
				recvLen += recv(client, buffer+recvLen, 4-recvLen, 0);
			}
			int respone;
			memcpy(&respone, buffer, 4);
			//std::cout << "send data: " << sendLen << " " << respone << std::endl;
			delete buffer;
		}
	}
}

bool CommandCommunicator::isReady()
{
	return commandMaker != nullptr && imgProcessor != nullptr;
}
