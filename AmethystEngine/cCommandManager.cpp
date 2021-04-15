#include "cCommandManager.hpp"



#pragma region SINGLETION
cCommandManager cCommandManager::stonCmdMngr;
cCommandManager* cCommandManager::GetCommandManager() { return &(cCommandManager::stonCmdMngr); }
cCommandManager::cCommandManager() {
	std::cout << "Command Manager Created" << std::endl; 
	pMediator = nullptr;
}
#pragma endregion



#pragma region MEDIATOR_INSTANTIATION
void cCommandManager::setMediatorPointer(iMediatorInterface* pMediator) { this->pMediator = pMediator; }
#pragma endregion



#pragma region COMMAND_INSTANTIATION
iCommand* cCommandManager::CreateAnimaScript(cmdType cmd) {
	iCommand* cmdscrp = nullptr;
	switch (cmd) {
	case ANIMASCRIPT:
		cmdscrp = new cCommandGroup();
		while (!_cmd_queue.empty()) {
			cmdscrp->AddSerialCommand(_cmd_queue.front());
			_cmd_queue.pop();
		}
		_map_script_to_cmd[data.getTarget()] = cmdscrp;
		break;

	default:
		break;
	}
	return cmdscrp;
}

iCommand* cCommandManager::CreateCommandGroup(cmdType cmd) {
	iCommand* cmdgrp = nullptr;
	switch (cmd) {
	case SERIAL:
		// transfer everything into the serial command groups cmd queue put the cmd group into a cmdgroup queue inside the cmd manager
		cmdgrp = new cCommandGroup();
		while(!_cmd_list.empty()) {
			cmdgrp->AddSerialCommand(_cmd_list.back());
			_cmd_list.pop_back();
		}
		_cmd_queue.push(cmdgrp);
		break;
			
	case PARALLEL:
		// transfer everything into the serial command groups cmd queue put the cmd group into a cmdgroup queue inside the cmd manager
		cmdgrp = new cCommandGroup();
		while(!_cmd_list.empty()) {
			cmdgrp->AddParallelCommand(_cmd_list.back());
			_cmd_list.pop_back();
		}
		_cmd_queue.push(cmdgrp);
		break;

	default:
		break;
	}
	// this can be used to gather commands and then put them in thier respective containers...
	// use this to set the cmd groups in the animation script cmd...
	return cmdgrp;
}

// fix so that the pointer to the command is returned and not set to the predefined types
iCommand* cCommandManager::CreateCommand(cmdType cmd_type) {
	iCommand* cmd = nullptr;
	sData cmddata;
	switch (cmd_type) {
	case MOVE:
		cmd = new cMove();
		cmddata = data;
		cmd->Init(cmddata);
		_cmd_list.insert(_cmd_list.begin(), cmd);
		break;

	case ROTATE:
		cmd = new cRotate();
		cmddata = data;
		cmd->Init(cmddata);
		_cmd_list.insert(_cmd_list.begin(), cmd);
		break;

	case FOLLOW:
		cmd = new cFollow();
		cmddata = data;
		cmd->Init(cmddata);
		_cmd_list.insert(_cmd_list.begin(), cmd);
		break;

	case TRIGGER:
		cmd = new cTrigger();
		cmddata = data;
		cmd->Init(cmddata);
		_cmd_list.push_back(cmd);
		break;

	case LOOKAT:
		cmd = new cLookAt();
		cmddata = data;
		cmd->Init(cmddata);
		_cmd_list.insert(_cmd_list.begin(), cmd);
		break;

	default:
		break;
	}
	return cmd;
}

void cCommandManager::CreateCommands() {
	m_str_to_enum[vec_move_to_cmd_strs] = MOVE;
	m_str_to_enum[vec_rotate_to_cmd_strs] = ROTATE;
	//cmdMove = new cMove();
	//cmdRotate = new cRotate();
}
#pragma endregion



#pragma region COMMAND_UTILITIES
void cCommandManager::ConvertToLower(std::string cmdstr) {
	std::string s;
	if (!cmdstr.empty()) {
		if (cmdstr.front() == '`')
			cmdstr = cmdstr.substr(1, cmdstr.back());
		for (char c : cmdstr)
			s += std::tolower(c);
		cmdstr = s;
	}
}

void cCommandManager::IssueCommand(std::string cmd) {
	sNVData data;
	_cmdstr = cmd;

	std::string temp;

	// parse cmd for data population
	std::vector<std::string> vdata;

	bool targetEntry = false;
	for (char c : cmd) {
		if (c == '.') {
			vdata.push_back(temp);
			temp = "";
		}
		else if (c == '"') {
			if (vdata.size() == 0)
				vdata.push_back("");
			if (targetEntry == false)
				targetEntry = true;
			else {
				if (vdata.size() == 0)
					vdata.push_back("");
				targetEntry = false;
				vdata.push_back(temp);
				temp = "";
			}
		}
		else if (c == ' ') {
			if (vdata.size() == 0)
				vdata.push_back("");
			if (!temp.empty())
				vdata.push_back(temp);
			temp = "";
		}
		else {
			temp += c;
		}
	}

	if (!temp.empty()) {
		vdata.push_back(temp);
		temp = "";
	}

	// SPECIAL SINGLE TYPE COMMANDS
	if (vdata.size() == 1) {
		ConvertToLower(vdata[0]);
		data.cmd = vdata[0];
	}

	else if (vdata[1] == "savefiles") {
		ConvertToLower(vdata[1]);
		data.cmd = vdata[1];
		data.target = "M";
		data._models_filename = vdata[2];
		data._lights_filename = vdata[3];
		data._complex_filename = vdata[4];
	}

	else if (vdata[1] == "control") {
		ConvertToLower(vdata[0]);
		ConvertToLower(vdata[1]);
		data.subsystem = vdata[0];
		data.cmd = vdata[1];
	}

	// cmd: data [show] [hide] [toggle]
	else if (vdata[1] == "data") {
		ConvertToLower(vdata[1]);
		ConvertToLower(vdata[2]);
		data.subsystem = "";
		data.cmd = vdata[1];
		data.state = vdata[2];
	}

	else if (vdata[1] == "getposition") {
		ConvertToLower(vdata[1]);
		data.subsystem = "";
		data.cmd = vdata[1];
		data.target = vdata[2];
	}

	else {
		if (vdata[1] == "create") {
			if (vdata[0] == "model") {
				ConvertToLower(vdata[0]);
				ConvertToLower(vdata[1]);

				data.subsystem = vdata[0];
				data.cmd = vdata[1];
				data.mesh = vdata[2];
				data.name = vdata[3];
			}
			else if (vdata[0] == "light") {
				ConvertToLower(vdata[0]);
				ConvertToLower(vdata[1]);

				data.subsystem = vdata[0];
				data.cmd = vdata[1];
				data.name = vdata[2];
			}
		}
		else if (vdata.size() == 3) {
			ConvertToLower(vdata[0]);
			ConvertToLower(vdata[1]);
			ConvertToLower(vdata[2]);

			if (vdata[0] != "")
				data.subsystem = vdata[0];
			data.cmd = vdata[1];
			data.state = vdata[2];
		}
		else if (vdata[0] != "") {
			if (vdata[0] != "") {
				ConvertToLower(vdata[0]);

				data.subsystem = vdata[0];
			}

			ConvertToLower(vdata[1]);
			data.cmd = vdata[1];
			data.target = vdata[2];

			if (vdata.size() == 4) {
				ConvertToLower(vdata[3]);
				data.state = vdata[3];
			}
		}
		else {
			ConvertToLower(vdata[0]);
			ConvertToLower(vdata[1]);
			ConvertToLower(vdata[3]);

			data.cmd = vdata[1];
			data.state = vdata[3];
			data.target = vdata[2];
			data.subsystem = vdata[0];
		}
	}
	this->RecieveMessage(data);
}

void cCommandManager::IssueCommand(sData& data) { this->RecieveMessage(data); }
#pragma endregion



#pragma region MEDIATOR_COMMUNICATIONS
sNVData cCommandManager::RecieveMessage(sNVData message) {
	sNVData response;
	response.status = "OK";

	if(message.status == "OK") { }
	else if(message.status == "NOK") { }

	// DEVCON PRINT OUT
	// This should probably be changed and filled with more information
	if (pMediator->RecieveMessage(message).status == "NOK")
		_cmd_result = "UNKNOWN COMMAND: " + _cmdstr;
	else
		_cmd_result = "COMMAND ACCEPTED: " + _cmdstr;

	_cmdstr = "";
	return response;
}




sData cCommandManager::RecieveMessage(sData& data){
	switch (pMediator->RecieveMessage(data).getResult()) {
		case OK:
			// command execution successfull...
			break;

		case UNKNOWN_SUBSYSTEM:
			// unknown subsystem specified for command execution, command execution failed...
			break;

		case UNKNOWN_COMMAND:
			// unknown command specified for command execution, command execution failed...
			break;

		case INVALID_COMMAND:
			// invalid command specified for specified subsystem, command execution failed...
			break;

		case NOK:
			// unknown error caught while executing command, command execution failed...
			break;

		default:
			data.setResult(NOK);
			// Feeney style oh no error!
			std::cout << "OH NO!!!!!!!!" << std::endl;
			// it should never reach this case at all, this is a catastrophic error...
			break;
	}
	return data;
}
#pragma endregion
