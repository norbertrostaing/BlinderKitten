#include "OSCInterface.h"
/*
  ==============================================================================

	OSCInterface.cpp
	Created: 26 Sep 2020 1:52:46pm
	Author:  bkupe

  ==============================================================================
*/

OSCInterface::OSCInterface() :
	Interface(getTypeString()),
	Thread("OSCZeroconf"),
	localPort(nullptr),
	servus("_osc._udp"),
	receiveCC(nullptr)
{
	internalMappings = addBoolParameter("Internal mappings", "parse organic and blinderkitten mappings", false);
	internalFeedbacks = addBoolParameter("Internal feedbacks", "send all internal feedbacks to outputs", false);

	receiveCC.reset(new EnablingControllableContainer("OSC Input"));
	receiveCC->customGetEditorFunc = &OSCInputEditor::create;
	addChildControllableContainer(receiveCC.get());

	localPort = receiveCC->addIntParameter("Local Port", "Local Port to bind to receive OSC Messages", 13000, 1024, 65535);
	localPort->warningResolveInspectable = this;

	receiver.registerFormatErrorHandler(&OSCHelpers::logOSCFormatError);
	receiver.addListener(this);

	if (!Engine::mainEngine->isLoadingFile) setupReceiver();

	//Send
	outputManager.reset(new BaseManager<OSCOutput>("OSC Outputs"));
	outputManager->addBaseManagerListener(this);

	addChildControllableContainer(outputManager.get());

	outputManager->setCanBeDisabled(true);
	if (!Engine::mainEngine->isLoadingFile)
	{
		OSCOutput* o = outputManager->addItem(nullptr, var(), false);
		o->remotePort->setValue(13001);
		if (!Engine::mainEngine->isLoadingFile) setupSenders();
	}

	addChildControllableContainer(&mappingManager);
	addChildControllableContainer(&feedbackManager);

	//Script
	scriptObject.getDynamicObject()->setMethod("send", OSCInterface::sendOSCFromScript);
	scriptObject.getDynamicObject()->setMethod("sendTo", OSCInterface::sendOSCToFromScript);
	scriptObject.getDynamicObject()->setMethod("match", OSCInterface::matchOSCAddrFromScript);
	scriptObject.getDynamicObject()->setMethod("register", OSCInterface::registerOSCCallbackFromScript);

	//scriptManager->scriptTemplate += ChataigneAssetManager::getInstance()->getScriptTemplate("osc");

	genericSender.connect("0.0.0.0", 1);
}

OSCInterface::~OSCInterface()
{
	if (isThreadRunning())
	{
		signalThreadShouldExit();
		waitForThreadToExit(1000);
		stopThread(100);
	}
}

void OSCInterface::setupReceiver()
{
	receiver.disconnect();
	if (receiveCC == nullptr) return;

	if (!receiveCC->enabled->boolValue())
	{
		localPort->clearWarning();
		return;
	}

	//DBG("Local port set to : " << localPort->intValue());
	bool result = receiver.connect(localPort->intValue());

	if (result)
	{
		NLOG(niceName, "Now receiving on port : " + localPort->stringValue());
		if (!isThreadRunning() && !Engine::mainEngine->isLoadingFile) startThread();

		Array<IPAddress> ad;

		IPAddress::findAllAddresses(ad);
		Array<String> ips;
		for (auto& a : ad) ips.add(a.toString());
		ips.sort();
		String s = "Local IPs:";
		for (auto& ip : ips) s += String("\n > ") + ip;

		NLOG(niceName, s);
		localPort->clearWarning();
	}
	else
	{
		NLOGERROR(niceName, "Error binding port " + localPort->stringValue());
		localPort->setWarningMessage("Error binding port " + localPort->stringValue());
	}

}


void OSCInterface::processMessage(const OSCMessage& msg)
{
	if (logIncomingData->boolValue())
	{
		String s = "";
		for (auto& a : msg) s += String(" ") + OSCHelpers::getStringArg(a);
		NLOG(niceName, msg.getAddressPattern().toString() << " :" << s);
	}

	processMessageInternal(msg);

	if (internalMappings->boolValue()) {
		UserInputManager::getInstance()->processMessage(msg, "");
	}

	/*
	if (scriptManager->items.size() > 0)
	{
		Array<var> params;
		params.add(msg.getAddressPattern().toString());
		var args = var(Array<var>()); //initialize force array
		for (auto& a : msg) args.append(OSCHelpers::argumentToVar(a));
		params.add(args);
		scriptManager->callFunctionOnAllItems(oscEventId, params);

		for (auto& entry : scriptCallbacks)
			if (std::get<0>(entry).matches(msg.getAddressPattern().toString()))
				scriptManager->callFunctionOnAllItems(std::get<1>(entry), params);
	}
	*/
}

void OSCInterface::processMessageInternal(const OSCMessage& m)
{
	for (OSCMapping* mapping : mappingManager.items) {
		mapping->processMessage(m);
	}
}

void OSCInterface::itemAdded(OSCOutput* output)
{
	output->warningResolveInspectable = this;
}

void OSCInterface::setupSenders()
{
	for (auto& o : outputManager->items) o->setupSender();
}

void OSCInterface::sendOSC(const OSCMessage& msg, String ip, int port)
{
	if (isClearing || outputManager == nullptr) return;
	if (!enabled->boolValue()) return;

	if (!outputManager->enabled->boolValue()) return;

	if (logOutgoingData->boolValue())
	{
		NLOG(niceName, "Send OSC : " << msg.getAddressPattern().toString());
		for (auto& a : msg)
		{
			LOG(OSCHelpers::getStringArg(a));
		}
	}

	if (ip.isNotEmpty() && port > 0)
	{
		genericSender.sendToIPAddress(ip, port, msg);
	}
	else
	{
		for (auto& o : outputManager->items) o->sendOSC(msg);
	}
}

void OSCInterface::feedback(String address, var value, String origin)
{
	feedbackManager.feedback(address, value, origin, logOutgoingData->boolValue());

	if (internalFeedbacks->boolValue()) {
		if (value.isString()) {
			address += "/name";
		}
		var sentVal;
		if (sentValues.contains(address)) {
			sentVal = sentValues.getReference(address);
		}
		if (sentVal != value) {
			sentValues.set(address, value);
			OSCMessage msg(address);
			msg.addArgument(OSCHelpers::varToArgument(value, OSCHelpers::TF));
			sendOSC(msg);
		}
	}
}

void OSCInterface::setupZeroConf()
{
	if (Engine::mainEngine->isClearing || localPort == nullptr) return;

	String nameToAdvertise;
	int portToAdvertise = 0;
	while (nameToAdvertise != niceName || portToAdvertise != localPort->intValue())
	{
		nameToAdvertise = niceName;
		portToAdvertise = localPort->intValue();

		servus.withdraw();

		if (!receiveCC->enabled->boolValue()) return;

		//DBG("ADVERTISE");
		servus.announce(portToAdvertise, ("Chataigne - " + nameToAdvertise).toStdString());

		if (nameToAdvertise != niceName || localPort->intValue() != portToAdvertise || !receiveCC->enabled->boolValue())
		{
			//DBG("Name or port changed during advertise, readvertising");
		}
	}

	NLOG(niceName, "Zeroconf service created : " << nameToAdvertise << ":" << portToAdvertise);
}

var OSCInterface::sendOSCFromScript(const var::NativeFunctionArgs& a)
{
	OSCInterface* m = getObjectFromJS<OSCInterface>(a);
	if (!m->enabled->boolValue()) return var();

	if (!checkNumArgs(m->niceName, a, 1)) return var();

	try
	{
		OSCMessage msg(a.arguments[0].toString());

		for (int i = 1; i < a.numArguments; ++i)
		{
			if (a.arguments[i].isArray())
			{
				Array<var>* arr = a.arguments[i].getArray();
				for (auto& aa : *arr) msg.addArgument(OSCHelpers::varToArgument(aa, OSCHelpers::TF));
			}
			else
			{
				msg.addArgument(OSCHelpers::varToArgument(a.arguments[i], OSCHelpers::TF));
			}
		}

		m->sendOSC(msg);
	}
	catch (OSCFormatError& e)
	{
		NLOGERROR(m->niceName, "Error sending message : " << e.description);
	}


	return var();
}

var OSCInterface::sendOSCToFromScript(const var::NativeFunctionArgs& a)
{
	OSCInterface* m = getObjectFromJS<OSCInterface>(a);
	if (!m->enabled->boolValue()) return var();
	if (!checkNumArgs(m->niceName, a, 3)) return var();

	try
	{
		OSCMessage msg(a.arguments[2].toString());

		for (int i = 3; i < a.numArguments; ++i)
		{
			if (a.arguments[i].isArray())
			{
				Array<var>* arr = a.arguments[i].getArray();
				for (auto& aa : *arr) msg.addArgument(OSCHelpers::varToArgument(aa, OSCHelpers::TF));
			}
			else
			{
				msg.addArgument(OSCHelpers::varToArgument(a.arguments[i], OSCHelpers::TF));
			}
		}

		m->sendOSC(msg, a.arguments[0], a.arguments[1]);
	}
	catch (OSCFormatError& e)
	{
		NLOGERROR(m->niceName, "Error sending message : " << e.description);
	}


	return var();
}

var OSCInterface::matchOSCAddrFromScript(const var::NativeFunctionArgs& a)
{
	OSCInterface* m = getObjectFromJS<OSCInterface>(a);
	if (!checkNumArgs(m->niceName, a, 2)) return var();

	try
	{
		OSCAddress address(a.arguments[0].toString());
		OSCAddressPattern pattern(a.arguments[1].toString());

		return pattern.matches(address);
	}
	catch (OSCFormatError& e)
	{
		NLOGERROR(m->niceName, "match() function called with incorrect parameters: " << e.description);
	}


	return var();
}

var OSCInterface::registerOSCCallbackFromScript(const var::NativeFunctionArgs& a)
{
	OSCInterface* m = getObjectFromJS<OSCInterface>(a);
	if (!checkNumArgs(m->niceName, a, 2)) return var();

	try
	{
		OSCAddressPattern pattern(a.arguments[0].toString());

		if (!Identifier::isValidIdentifier(a.arguments[1].toString()))
		{
			NLOGERROR(m->niceName, "register() function: invalid callback name " << a.arguments[1].toString());
			return var();
		}
		Identifier callbackName(a.arguments[1].toString());

		for (auto& i : m->scriptCallbacks)
			if (pattern == std::get<0>(i) && callbackName == std::get<1>(i))
				return var();

		m->scriptCallbacks.add(std::make_tuple(pattern, callbackName));
	}
	catch (OSCFormatError& e)
	{
		NLOGERROR(m->niceName, "register() function: invalid pattern: " << e.description);
	}

	return var();
}


void OSCInterface::loadJSONDataInternal(var data)
{
	Interface::loadJSONDataInternal(data);
	//if (receiveCC != nullptr) receiveCC->loadJSONData(data.getProperty("input", var()));
	if (outputManager != nullptr)
	{
		//outputManager->loadJSONData(data.getProperty("outputs", var()));
		setupSenders();
	}

	if (!isThreadRunning()) startThread();
	setupReceiver();
}


void OSCInterface::onContainerNiceNameChanged()
{
	Interface::onContainerNiceNameChanged();
	if (Engine::mainEngine->isLoadingFile || Engine::mainEngine->isClearing) return;
	if (!isThreadRunning()) startThread();
}

void OSCInterface::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	Interface::onControllableFeedbackUpdateInternal(cc, c);

	if (c == outputManager->enabled)
	{
		//bool rv = receiveCC != nullptr ? receiveCC->enabled->boolValue() : false;
		bool sv = outputManager->enabled->boolValue();
		for (auto& o : outputManager->items) o->setForceDisabled(!sv);

	}
	else if (c == receiveCC->enabled)
	{
		bool rv = receiveCC->enabled->boolValue();
		//bool sv = outputManager != nullptr ? outputManager->enabled->boolValue() : false;
		localPort->setEnabled(rv);
		if (!isCurrentlyLoadingData) setupReceiver();

	}
	else if (c == localPort)
	{
		if (!isCurrentlyLoadingData) setupReceiver();
	}
}

void OSCInterface::oscMessageReceived(const OSCMessage& message)
{
	if (!enabled->boolValue()) return;
	processMessage(message);
}

void OSCInterface::oscBundleReceived(const OSCBundle& bundle)
{
	if (!enabled->boolValue()) return;
	for (auto& m : bundle)
	{
		processMessage(m.getMessage());
	}
}

void OSCInterface::run()
{
	setupZeroConf();
}


///// OSC OUTPUT

OSCOutput::OSCOutput() :
	BaseItem("OSC Output"),
	Thread("OSC output"),
	forceDisabled(false),
	senderIsConnected(false)
{
	isSelectable = false;

	useLocal = addBoolParameter("Local", "Send to Local IP (127.0.0.1). Allow to quickly switch between local and remote IP.", true);
	remoteHost = addStringParameter("Remote Host", "Remote Host to send to.", "127.0.0.1");
	remoteHost->autoTrim = true;
	remoteHost->setEnabled(!useLocal->boolValue());
	remotePort = addIntParameter("Remote port", "Port on which the remote host is listening to", 9000, 1024, 65535);

	if (!Engine::mainEngine->isLoadingFile) setupSender();
}

OSCOutput::~OSCOutput()
{
	signalThreadShouldExit();
	waitForThreadToExit(1000);
	stopThread(1000);
}

void OSCOutput::setForceDisabled(bool value)
{
	if (forceDisabled == value) return;
	forceDisabled = value;
	setupSender();
}

void OSCOutput::onContainerParameterChangedInternal(Parameter* p)
{

	if (p == remoteHost || p == remotePort || p == useLocal)
	{
		if (!Engine::mainEngine->isLoadingFile) setupSender();
		if (p == useLocal) remoteHost->setEnabled(!useLocal->boolValue());
	}
	else if (p == enabled)
	{
		if (!Engine::mainEngine->isLoadingFile) setupSender();
	}
}

InspectableEditor* OSCOutput::getEditorInternal(bool isRoot)
{
	return new OSCOutputEditor(this, isRoot);
}

void OSCOutput::setupSender()
{
	signalThreadShouldExit();
	notify();
	waitForThreadToExit(1000);

	senderIsConnected = false;
	sender.disconnect();

	if (isThreadRunning())
	{
		stopThread(1000);
		// Clear queue
		const ScopedLock sl(queueLock);
		while (!messageQueue.empty())
			messageQueue.pop();
	}

	if (!enabled->boolValue() || forceDisabled || Engine::mainEngine->isClearing) return;

	String targetHost = useLocal->boolValue() ? "127.0.0.1" : remoteHost->stringValue();
	senderIsConnected = sender.connect(targetHost, remotePort->intValue());
	if (senderIsConnected)
	{
		startThread();

		NLOG(niceName, "Now sending to " + remoteHost->stringValue() + ":" + remotePort->stringValue());
		clearWarning();
	}
	else
	{
		NLOGWARNING(niceName, "Could not connect to " << remoteHost->stringValue() << ":" + remotePort->stringValue());
		setWarningMessage("Could not connect to " + remoteHost->stringValue() + ":" + remotePort->stringValue());
	}
}

void OSCOutput::sendOSC(const OSCMessage& m)
{
	if (!enabled->boolValue() || forceDisabled || !senderIsConnected) return;

	{
		const ScopedLock sl(queueLock);
		messageQueue.push(std::make_unique<OSCMessage>(m));
	}
	notify();
}

void OSCOutput::run()
{
	while (!Engine::mainEngine->isClearing && !threadShouldExit())
	{
		std::unique_ptr<OSCMessage> msgToSend;

		{
			const ScopedLock sl(queueLock);
			if (!messageQueue.empty())
			{
				msgToSend = std::move(messageQueue.front());
				messageQueue.pop();
			}
		}

		if (msgToSend)
			sender.send(*msgToSend);
		else
			wait(1000); // notify() is called when a message is added to the queue
	}

	// Clear queue
	const ScopedLock sl(queueLock);
	while (!messageQueue.empty())
		messageQueue.pop();
}
