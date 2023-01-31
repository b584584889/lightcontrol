
//
// https://github.com/guotianqing/RmqMgr
//
#include "RmqMgrBase.h"

#include <boost/make_shared.hpp>
#include <boost/bind/bind.hpp>
// #include <boost/bind.hpp>
#include <functional>
#include <thread>
#include <chrono>

using namespace std;

RmqMgrBase::RmqMgrBase()
{
	m_connection = nullptr;
	m_channel = nullptr;
	m_channelPub = nullptr;
	m_isAutoResume = false;
	m_pTcpMgr = nullptr;
}

RmqMgrBase::~RmqMgrBase()
{
}

bool RmqMgrBase::MqInfoInit(const MqInfo &mqInfo, string &err)
{
	try
	{
		m_mqInfo = mqInfo;
	}
	catch (const std::exception &e)
	{
		err.assign( string(e.what()));
		return false;
	}

	return true;
}

bool RmqMgrBase::StartMqInstance(string &err)
{
	try
	{
		m_pTcpMgr = TcpMgr::Init(m_mqInfo, this);

		if (!m_pTcpMgr->WaitForReady())
		{
			err.assign("Rmq ");
			OnRtnErrMsg(err);
			return false;
		}

		// get the connection
		GetMqConnection();

		// make a channel
		if (!CreateMqChannel(err))
		{
			OnRtnErrMsg(err);
			OnStatusChange(false);
			return false;
		}
	}
	catch (const std::exception &e)
	{
		err.assign( string(e.what()));
		OnRtnErrMsg(err);
		return false;
	}

	return true;
}

bool RmqMgrBase::ReleaseMqInstance(string &err, const bool m_isAutoResume)
{
	try
	{
		if (!m_isAutoResume)
		{
		}

		if (!CloseMqChannel(err))
		{
			return false;
		}

		if (!CloseMqConnection(err))
		{
			return false;
		}

		m_pTcpMgr->Finit();
	}
	catch (const std::exception &e)
	{
		err.assign( string(e.what()));
		return false;
	}

	return true;
}

void RmqMgrBase::GetMqConnection()
{
	if (!m_pTcpMgr)
	{
		string err;
		if (!StartMqInstance(err))
		{
			OnRtnErrMsg(err);
			return;
		}
	}
	else
	{
		m_connection = m_pTcpMgr->GetConnection();
	}
}

bool RmqMgrBase::CloseMqConnection(string &err)
{
	try
	{
		if (m_connection->usable())
		{
			bool ret = m_connection->close();
			if (!ret)
			{
				err.assign(" ");
				return ret;
			}
		}
	}
	catch (const std::exception &e)
	{
		err.assign( string(e.what()));
		return false;
	}

	return true;
}

bool RmqMgrBase::CreateMqChannel(string &err)
{
	try
	{
		if (!m_connection || !m_connection->usable())
		{
			GetMqConnection();
		}

		// std::cout << "RmqMgrBase::CreateMqChannel() "<< std::endl;

		m_channel = boost::make_shared<AMQP::Channel>(m_connection.get());

		m_channel->onError(std::bind(&RmqMgrBase::ChannelErrCb, this, std::placeholders::_1));
		m_channel->onReady(std::bind(&RmqMgrBase::ChannelOkCb, this));
	}
	catch (const std::exception &e)
	{
		err.assign( string(e.what()));
		return false;
	}

	return true;
}

void RmqMgrBase::ChannelErrCb(const char *msg)
{
	string err;
	err =  string(msg);
	OnRtnErrMsg(err);
	OnStatusChange(false);
	std::this_thread::sleep_for(1s);
	CreateMqChannel(err);
}

void RmqMgrBase::ChannelOkCb()
{
	// std::cout << "RmqMgrBase::ChannelOkCb()" << std::endl;

	OnStatusChange(true);
	CreateMqPubChannel();
	string err;
	if (!CreateMqExchange(m_mqInfo.exchangeName, m_mqInfo.exchangeType, err))
	{
		OnRtnErrMsg(err);
	}

	if (!CreateMqQueue(m_mqInfo.queueName, err))
	{
		OnRtnErrMsg(err);
	}

	if (!BindQueue(m_mqInfo.queueName, m_mqInfo.exchangeName, m_mqInfo.bindingKey, err))
	{
		OnRtnErrMsg(err);
	}

	if (!StartConsumeMsg(m_mqInfo.queueName, err))
	{
		OnRtnErrMsg(err);
	}
}

bool RmqMgrBase::CloseMqChannel(string &err)
{
	try
	{
		if (m_channel->usable())
		{
			m_channel->close()
				.onError(std::bind(&RmqMgrBase::ChannelCloseErrCb, this, std::placeholders::_1));
		}
		if (m_channelPub->usable())
		{
			m_channelPub->close()
				.onError(std::bind(&RmqMgrBase::ChannelCloseErrCb, this, std::placeholders::_1));
		}
	}
	catch (const std::exception &e)
	{
		err.assign( string(e.what()));
		return false;
	}

	return true;
}

void RmqMgrBase::ChannelCloseErrCb(const char *msg)
{
	string err;
	err =   string(msg);
	OnRtnErrMsg(err);
}

bool RmqMgrBase::CreateMqExchange(const string &exchangeName, const string &exchangeType, string &err)
{
	try
	{
		AMQP::ExchangeType type;
		if (exchangeType == "topic")
		{
			type = AMQP::topic;
		}
		else if (exchangeType == "direct")
		{
			type = AMQP::direct;
		}
		else if (exchangeType == "fanout")
		{
			type = AMQP::fanout;
		}
		else if (exchangeType == "headers")
		{
			type = AMQP::headers;
		}
		else if (exchangeType == "consistent_hash")
		{
			type = AMQP::consistent_hash;
		}
		else
		{
			err.assign( exchangeType);
			return false;
		}

		m_channel->declareExchange(exchangeName, type)
			.onError(std::bind(&RmqMgrBase::CreatMqExchangeErrCb, this, std::placeholders::_1));
	}
	catch (const std::exception &e)
	{
		err.assign( string(e.what()));
		return false;
	}

	return true;
}

void RmqMgrBase::CreatMqExchangeErrCb(const char *msg)
{
	string err;
	err =  string(msg);
	OnRtnErrMsg(err);
	OnStatusChange(false);

	CreateMqChannel(err);
}

bool RmqMgrBase::CreateMqQueue(const string &queueName, string &err)
{
	try
	{
		m_channel->declareQueue(queueName, AMQP::exclusive)
			.onError(std::bind(&RmqMgrBase::CreatMqQueueErrCb, this, std::placeholders::_1));
	}
	catch (const std::exception &e)
	{
		err.assign( string(e.what()));
		return false;
	}

	return true;
}

void RmqMgrBase::CreatMqQueueErrCb(const char *msg)
{
	string err;
	err =  string(msg);
	OnRtnErrMsg(err);
	OnStatusChange(false);

	CreateMqChannel(err);
}

bool RmqMgrBase::BindQueue(const string &queueName, const string &exchangeName, const string &bindingKey, string &err)
{
	try
	{
		m_channel->bindQueue(exchangeName, queueName, bindingKey)
			.onError(std::bind(&RmqMgrBase::BindQueueErrCb, this, std::placeholders::_1));
	}
	catch (const std::exception &e)
	{
		err.assign( string(e.what()));
		return false;
	}

	return true;
}

void RmqMgrBase::BindQueueErrCb(const char *msg)
{
	string err;
	err = string(msg);
	OnRtnErrMsg(err);
	OnStatusChange(false);

	CreateMqChannel(err);
}

bool RmqMgrBase::SetQosValue(const uint16_t val, string &err)
{
	try
	{
		m_channel->setQos(val)
			.onError(std::bind(&RmqMgrBase::SetQosValueErrCb, this, std::placeholders::_1));
	}
	catch (const std::exception &e)
	{
		err.assign( string(e.what()));
		return false;
	}
	return true;
}

void RmqMgrBase::SetQosValueErrCb(const char *msg)
{
	string err;
	err =  string(msg);
	OnRtnErrMsg(err);
	OnStatusChange(false);

	CreateMqChannel(err);
}

void RmqMgrBase::CreateMqPubChannel()
{
	try
	{
		// std::cout << "RmqMgrBase::CreateMqPubChannel()" << std::endl;

		m_channelPub = boost::make_shared<AMQP::Channel>(m_connection.get());
		m_channelPub->onError([this](const char *message) {
			string err;
			err =  string(message);
			OnRtnErrMsg(err);
			OnStatusChange(false);
		});
	}
	catch (const std::exception &e)
	{
		string err(" ");
		err.append(e.what());
		OnRtnErrMsg(err);
		OnStatusChange(false);
	}
}

bool RmqMgrBase::PublishMsg(const string &msg, string &err)
{
	try
	{
		// TODO:
		// AMQP::Channel channel(m_connection.get());

		std::cout << "RmqMgrBase::PublishMsg() msg:" << msg << " exchangeName:"<<m_mqInfo.exchangeName<< " routingKey:"<<m_mqInfo.routingKey<< std::endl;

		m_channelPub->publish(m_mqInfo.exchangeName, m_mqInfo.routingKey, msg);
	}
	catch (const std::exception &e)
	{
		err.assign( string(e.what()));
		return false;
	}

	return true;
}

bool RmqMgrBase::StartConsumeMsg(const string &queueName, string &err)
{
	try
	{
		m_channel->consume(queueName)
			.onReceived(std::bind(&RmqMgrBase::ConsumeRecvedCb, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
			.onError(std::bind(&RmqMgrBase::ConsumeErrorCb, this, std::placeholders::_1));
	}
	catch (const std::exception &e)
	{
		err.assign( string(e.what()));
		return false;
	}
	return true;
}

void RmqMgrBase::ConsumeRecvedCb(const AMQP::Message &message, uint64_t deliveryTag, bool redelivered)
{
	OnRecvedData(message.body(), message.bodySize());

	// acknowledge the message
	m_channel->ack(deliveryTag);
}

void RmqMgrBase::ConsumeErrorCb(const char *msg)
{
	string err;
	err =  string(msg);
	OnRtnErrMsg(err);
	OnStatusChange(false);

	CreateMqChannel(err);
}

// MyConnectionHandler
MyConnectionHandler::MyConnectionHandler(boost::shared_ptr<TcpMgr> pTcpMgr)
{
	m_pTcpMgr = pTcpMgr;
}

void MyConnectionHandler::onData(AMQP::Connection *connection, const char *data, size_t size)
{
	try
	{
		if (!m_pTcpMgr)
		{
			return;
		}
		string err;
		string msg(data, size);
		m_pTcpMgr->SendData(msg, err);
	}
	catch (const std::exception&e)
	{
		m_pTcpMgr->OnErrMsg(std::string(e.what()));
	}
}

void MyConnectionHandler::onReady(AMQP::Connection *connection)
{
	m_pTcpMgr->SetLoginReady();
}

void MyConnectionHandler::onError(AMQP::Connection *connection, const char *message)
{
	m_pTcpMgr->OnErrMsg(std::string(message));
}

void MyConnectionHandler::onClosed(AMQP::Connection *connection)
{
	m_pTcpMgr->OnErrMsg(std::string("OnErrMsg"));
}

TcpMgr::TcpMgr()
	: m_work(m_ios),
	m_strand(m_ios),
	m_sock(m_ios),
	m_socketStarted(false),
	m_is_ready(false)
{
}

boost::shared_ptr<TcpMgr> TcpMgr::Init(const MqInfo& mqInfo, RmqMgrBase *pRmqMgrBase)
{
	boost::shared_ptr<TcpMgr> newTcpMgr(new TcpMgr());
	newTcpMgr->Start(mqInfo, pRmqMgrBase);
	return newTcpMgr;
}

void TcpMgr::Run()
{
	/*auto id = std::this_thread::get_id();
	stringstream ss;
	ss << id;
	OnErrMsg("Rmq threadid: " + ss.str());*/
	m_ios.run();
}

void TcpMgr::Finit()
{
	CloseSocket();
	m_ios.stop();
	m_threads.join_all();
}

void TcpMgr::Start(const MqInfo& mqInfo, RmqMgrBase *pRmqMgrBase)
{
	m_mqInfo = mqInfo;
	m_pRmqMgrBase = pRmqMgrBase;
	m_pHandler = new MyConnectionHandler(shared_from_this());
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(m_mqInfo.ip), m_mqInfo.port);
	m_sock.async_connect(ep, m_strand.wrap(boost::bind(&TcpMgr::OnConnect, shared_from_this(), _1)));
	for (auto i = 0; i < kNumOfWorkThreads; ++i)
	{
		m_threads.create_thread(boost::bind(&TcpMgr::Run, this));
	}
}

void TcpMgr::OnConnect(const error_code &err)
{
	if (!err && m_sock.is_open())
	{
		m_socketStarted = true;

		m_pConnect = boost::make_shared<AMQP::Connection>(m_pHandler, AMQP::Login(m_mqInfo.loginName, m_mqInfo.loginPwd));

		RecvData();
	}
	else
	{
		OnErrMsg( err.message());
		m_socketStarted = false;
		ReConnectServer();
	}
}

void TcpMgr::RecvData(int32_t total_bytes)
{
	m_strand.post(boost::bind(&TcpMgr::DispatchRecv, shared_from_this(), total_bytes));
}

void TcpMgr::DispatchRecv(int32_t total_bytes)
{
	bool should_start_receive = m_pending_recvs.empty();
	m_pending_recvs.push_back(total_bytes);
	if (should_start_receive)
	{
		StartRecv(total_bytes);
	}
}

void TcpMgr::StartRecv(int32_t total_bytes)
{
	if (total_bytes > 0)
	{
		m_recv_buffer.resize(total_bytes);
		boost::asio::async_read(m_sock, boost::asio::buffer(m_recv_buffer), m_strand.wrap(boost::bind(&TcpMgr::HandleRecv, shared_from_this(), _1, _2)));
	}
	else
	{
		m_recv_buffer.resize(kReceiveBufferSize);
		m_sock.async_read_some(boost::asio::buffer(m_recv_buffer), m_strand.wrap(boost::bind(&TcpMgr::HandleRecv, shared_from_this(), _1, _2)));
	}
}

void TcpMgr::HandleRecv(const error_code &err, size_t bytes)
{
	if (!err)
	{
		m_recv_buffer.resize(bytes);
		{
			//std::lock_guard<std::mutex> guard(m_lock);
			m_parseBuf.insert(m_parseBuf.end(), m_recv_buffer.begin(), m_recv_buffer.end());
			ParseAmqpData();
		}
		m_pending_recvs.pop_front();
		if (!m_pending_recvs.empty())
		{
			StartRecv(m_pending_recvs.front());
		}
		else
		{
			RecvData();
		}
	}
	else
	{
		OnErrMsg( err.message());
		CloseSocket();
		ReConnectServer();
	}
}

void TcpMgr::ParseAmqpData()
{
	try
	{
		auto data_size = m_parseBuf.size();
		size_t parsed_bytes = 0;
		auto expected_bytes = m_pConnect->expected();
		while (data_size - parsed_bytes >= expected_bytes)
		{
			std::vector<char> buff(m_parseBuf.begin() + parsed_bytes, m_parseBuf.begin() + parsed_bytes + expected_bytes);
			parsed_bytes += m_pConnect->parse(buff.data(), buff.size());
			expected_bytes = m_pConnect->expected();
		}
		m_parseBuf.erase(m_parseBuf.begin(), m_parseBuf.begin() + parsed_bytes);
	}
	catch (const std::exception&e)
	{
		OnErrMsg( string(e.what()));
	}
}

bool TcpMgr::SendData(const string &msg, std::string errmsg)
{
	try
	{
		auto pmsg(boost::make_shared<string>(msg));
		m_strand.post(boost::bind(&TcpMgr::SendMsg, shared_from_this(), pmsg));
	}
	catch (const std::exception &e)
	{
		errmsg.assign( string(e.what()));
		return false;
	}

	return true;
}

void TcpMgr::SendMsg(boost::shared_ptr<string> pmsg)
{
	bool should_start_send = m_pending_sends.empty();
	m_pending_sends.emplace_back(pmsg);
	if (should_start_send)
	{
		StartSend();
	}
}

void TcpMgr::StartSend()
{
	if (!m_pending_sends.empty())
	{
		boost::asio::async_write(m_sock, boost::asio::buffer(*m_pending_sends.front().get()), m_strand.wrap(boost::bind(&TcpMgr::HandleWrite, shared_from_this(), _1)));
	}
}

void TcpMgr::HandleWrite(const error_code &err)
{
	if (err)
	{
		OnErrMsg( err.message());
		CloseSocket();
		ReConnectServer();
	}
	else
	{
		m_pending_sends.pop_front();
		StartSend();
	}
}

void TcpMgr::ReConnectServer()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(kTcpRetryInterval));
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(m_mqInfo.ip), m_mqInfo.port);
	m_sock.async_connect(ep, m_strand.wrap(boost::bind(&TcpMgr::OnConnect, shared_from_this(), _1)));
}

void TcpMgr::CloseSocket()
{
	OnErrMsg(std::string(" "));
	m_strand.post(boost::bind(&TcpMgr::StartCloseSocket, shared_from_this()));
}

void TcpMgr::StartCloseSocket()
{
	if (!m_socketStarted)
	{
		return;
	}
	boost::system::error_code ec;
	m_sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
	m_sock.close(ec);
	m_socketStarted = false;
}

void TcpMgr::SetLoginReady()
{
	{
		std::lock_guard<std::mutex> lk(m_mtx_login);
		m_is_ready = true;
	}
	m_cv_login_succ.notify_all();
}

bool TcpMgr::WaitForReady()
{
	std::unique_lock<std::mutex> lk(m_mtx_login);
	return m_cv_login_succ.wait_for(lk, std::chrono::seconds(kLoginRmqTimeOut), [this] {
		return m_is_ready;
	});
}

boost::shared_ptr<AMQP::Connection> TcpMgr::GetConnection()
{
	return m_pConnect;
}

// void TcpMgr::OnErrMsg(std::string& msg)
void TcpMgr::OnErrMsg(std::string msg)
{
	m_pRmqMgrBase->OnRtnErrMsg(msg);
}
