#ifndef __PEERDOOR__
#define __PEERDOOR__

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif
#include <asio.hpp>
#include <memory>

/*
listens for peer connections.
When found passes them to the PeerMaster
*/

using namespace std;

namespace stellar
{
    class Application;

	class PeerDoor
	{
        Application &mApp;

		asio::ip::tcp::acceptor* mAcceptor;

		void acceptNextPeer();
		void handleKnock(shared_ptr<asio::ip::tcp::socket> pSocket);
	public:
		PeerDoor(Application &);

		void start();
		void close();

	};
}

#endif
