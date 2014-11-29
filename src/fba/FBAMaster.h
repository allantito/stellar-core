#ifndef __FBAMASTER__
#define __FBAMASTER__

#include <map>
#include "ledger/Ledger.h"
#include "txherder/TransactionSet.h"
#include "fba/QuorumSet.h"
#include "fba/Statement.h"
#include "fba/FBAGateway.h"
#include "fba/OurNode.h"

/*
There is one FBAMaster that oversees the consensus process

As we learn about transactions we add them to the mCollectingTransactionSet
negotiate with peers till we decide on the current tx set
send this tx set to ledgermaster to be applied
start next ledger close


What triggers ledgerclose?
	people on your UNL start to close
	you have tx and enough time has passed



When a ledger closes

FBA:
	we see a prepare msg from someone
	we issue our own prepare msg


*/

namespace stellar
{
    class Application;
    typedef std::shared_ptr<Application> ApplicationPtr;

	class FBAMaster : public FBAGateway
	{
        ApplicationPtr mApp;
		bool mValidatingNode;
		OurNode::pointer mOurNode;
		QuorumSet::pointer mOurQuorumSet;
        //NodePool mNodePool;

		// anytime we need a qset we store it here
		//LATER: map<stellarxdr::uint256, bool> mNeededQuorumSets;

		// map of nodes we have gotten FBA messages from in this round
		// we save ones we don't care about in case they are on some yet unknown Quorom Set
		map<stellarxdr::uint256, Node::pointer> mKnownNodes;

		// Statements we have gotten from the network but are waiting to get the txset of
		vector<Statement::pointer> mWaitTxStatements;

		// statements we have gotten with a ledger time too far in the future
		vector<Statement::pointer> mWaitFutureStatements;



		// Collect any FBA messages we get for the next slot in case people are closing before you are ready
		vector<Statement::pointer> mCollectedStatements;

		

		void checkRatify();
		void findPrepare();
		// returns true if our current prepare message is V Blocked
		bool checkVBlock();

		enum FBAState
		{
			WAITING,  // we committed the last ledger so fast that we should wait a bit before closing the next one
			UNPREPARED,
			PREPARED,
			RATIFIED,
			COMMITED
		};

		// make sure we only send out our own FBA messages if we are a validator

		bool processStatement(Statement::pointer statement);
		void processStatements(vector<Statement::pointer>& statementList);

	public:

		FBAMaster();
        void setApplication(ApplicationPtr app);

		void setValidating(bool validating){ mValidatingNode = validating; }

		void startNewRound(Ballot::pointer firstBallot);
		
		void transactionSetAdded(TransactionSet::pointer txSet);

		void addQuorumSet(QuorumSet::pointer qset);

        QuorumSet::pointer getOurQuorumSet() { return mOurQuorumSet; }

		

		// get a new statement from the network
		void recvStatement(Statement::pointer statement);

		void getMessage();  // get some msg from the network about closing

		void ledgerClosed();
	};
}

#endif