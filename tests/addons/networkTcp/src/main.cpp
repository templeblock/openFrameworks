#include "ofMain.h"
#include "ofAppNoWindow.h"
#include "ofxUnitTests.h"
#include "ofxNetwork.h"

class ofApp: public ofxUnitTestsApp{
public:
	void testNonBlocking(){
		ofLogNotice() << "---------------------------------------";
		ofLogNotice() << "testNonBlocking";

		int port = ofRandom(15000, 65535);

		ofxTCPServer server;
		test(server.setup(port,false), "non blocking server");

		ofxTCPClient client;
		test(client.setup("127.0.0.1",port,false), "non blocking client");

		bool received = false;
		std::string messageSent = "message";
		std::string messageReceived;

		test(client.send(messageSent), "send non blocking from client");
		for(int i=0;i<10 && !received;i++){
			ofSleepMillis(200);
			messageReceived = server.receive(0);
			if(messageReceived == messageSent){
				received = true;
			}
		}
		test(received, "receive non blocking from server");

		test(server.send(0,messageSent), "send non blocking from server");
		messageReceived = "";
		for(int i=0;i<10 && !received;i++){
			ofSleepMillis(200);
			messageReceived = client.receive();
			if(messageReceived == messageSent){
				received = true;
			}
		}
		test(received, "receive non blocking from client");
	}

	void testBlocking(){
		ofLogNotice() << "";
		ofLogNotice() << "---------------------------------------";
		ofLogNotice() << "testBlocking";

		int port = ofRandom(15000, 65535);

		ofxTCPServer server;
		test(server.setup(port,true), "blocking server");

		ofxTCPClient client;
		test(client.setup("127.0.0.1",port,true), "blocking client");

		std::string messageSent = "message";
		test(client.send(messageSent), "send blocking from client");
		test_eq(server.receive(0), messageSent, "receive blocking from server");
		test(server.send(0, messageSent), "send blocking from server");
		test_eq(client.receive(), messageSent, "receive blocking from client");
	}

	void disconnectionAutoDetection(){
		ofLogNotice() << "";
		ofLogNotice() << "---------------------------------------";
		ofLogNotice() << "disconnectionAutoDetection";

		int port = ofRandom(15000, 65535);

		ofxTCPServer server;
		test(server.setup(port,true), "blocking server");

		ofxTCPClient client;
		test(client.setup("127.0.0.1",port,true), "blocking client");

		test(server.isConnected(), "server is still connected");
		test(client.isConnected(), "client is still connected");

		test(server.disconnectClient(0), "server closes correctly");
		test(!client.isConnected(), "client detects disconnection");

		test(client.setup("127.0.0.1",port,true), "client reconnects");

		test(client.close(), "client disconnects");
		test(!server.isClientConnected(0), "server detects disconnection");
		test(!server.isClientConnected(1), "server detects disconnection");
	}

	void testSendRaw(){
		ofLogNotice() << "";
		ofLogNotice() << "---------------------------------------";
		ofLogNotice() << "testSendRaw";

		int port = ofRandom(15000, 65535);

		ofxTCPServer server;
		test(server.setup(port,true), "blocking server");

		ofxTCPClient client;
		test(client.setup("127.0.0.1", port, true), "blocking client");

		std::string messageSent = "message";
		test(client.sendRaw(messageSent), "send blocking from client");

		std::vector<char> messageReceived(messageSent.size()+1, 0);
		int received = 0;
		do{
			auto ret = server.receiveRawBytes(0, messageReceived.data() + received, messageSent.size());
			test(ret>0, "received blocking from server");
			if(ret>0){
				received += ret;
			}else{
				break;
			}
		}while(received<messageSent.size());

		test_eq(messageSent, std::string(messageReceived.data()), "messageSent == messageReceived");
	}

	void testSendRawBytes(){
		ofLogNotice() << "";
		ofLogNotice() << "---------------------------------------";
		ofLogNotice() << "testSendRawBytes";

		int port = ofRandom(15000, 65535);

		ofxTCPServer server;
		test(server.setup(port,true), "blocking server");

		ofxTCPClient client;
		test(client.setup("127.0.0.1", port, true), "blocking client");

		std::string messageSent = "message";
		test(client.sendRawBytes(messageSent.c_str(), messageSent.size()), "send blocking from client");

		std::vector<char> messageReceived(messageSent.size()+1, 0);
		int received = 0;
		do{
			auto ret = server.receiveRawBytes(0, messageReceived.data() + received, messageSent.size());
			test(ret>0, "received blocking from server");
			if(ret>0){
				received += ret;
			}else{
				break;
			}
		}while(received<messageSent.size());

		test_eq(messageSent, std::string(messageReceived.data()), "messageSent == messageReceived");
	}

	void run(){
		ofSeedRandom(ofGetSeconds());
		testNonBlocking();
		testBlocking();
		disconnectionAutoDetection();
		testSendRaw();
		testSendRawBytes();
	}
};

//========================================================================
int main( ){
	auto window = make_shared<ofAppNoWindow>();
	auto app = make_shared<ofApp>();
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(window, app);
	ofRunMainLoop();
}

