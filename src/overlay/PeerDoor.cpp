#include "PeerDoor.h"
#include "main/Application.h"
#include "Peer.h"
#include "overlay/PeerMaster.h"

namespace stellar
{
	PeerDoor::PeerDoor(Application &app)
        : mApp(app)
	{
        mAcceptor = NULL;
	}
	
	void PeerDoor::start(Application::pointer app)
	{
        mApp = app;
		mAcceptor = new asio::ip::tcp::acceptor(*mApp->getPeerMaster().mIOservice);
		asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), mApp->mConfig.PEER_PORT);
		mAcceptor->open(endpoint.protocol());
		mAcceptor->bind(endpoint);
		mAcceptor->listen();

		acceptNextPeer();
	}

	void PeerDoor::close()
	{
		mAcceptor->cancel();
	}

	void PeerDoor::acceptNextPeer()
	{
		shared_ptr<asio::ip::tcp::socket> pSocket = std::make_shared<asio::ip::tcp::socket>(*mApp->getPeerMaster().mIOservice);

		mAcceptor->async_accept(*pSocket, bind(&PeerDoor::handleKnock, this, pSocket));
	}

	void PeerDoor::handleKnock(shared_ptr<asio::ip::tcp::socket> socket)
	{
        LOG(DEBUG) << "PeerDoor handleKnock()";
		Peer::pointer peer = std::make_shared<Peer>(mApp, socket);
        mApp.getPeerMaster().addPeer(peer);
		peer->createFromDoor();
		acceptNextPeer();
	}
}

