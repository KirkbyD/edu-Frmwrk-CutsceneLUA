#pragma once
#include <iostream>
#include <map>
#include <queue>
#include "iMediatorInterface.hpp"
#include "iCommand.hpp"
#include "cCommandGroup.hpp"
#include "cMove.hpp"
#include "cRotate.hpp"
#include "cFollow.hpp"
#include "cTrigger.hpp"
#include "cLookAt.hpp"

// DEVCON GLOBALS (MAIN)
extern std::string _cmd_result;



class cCommandManager : iMediatorInterface {
	cCommandManager();
	static cCommandManager stonCmdMngr;
	iMediatorInterface* pMediator;

	void ConvertToLower(std::string s);

	std::string _cmdstr;

	std::vector<iCommand*> _cmd_list;
	std::queue<iCommand*> _cmd_queue;

	std::map<std::vector<std::string>, cmdType> m_str_to_enum;

	// mainly for devcon now???
	std::vector<std::string> vec_move_to_cmd_strs = {"moveto" , "MOVETO", "movetoxyz", "MOVETOXYZ" };
	std::vector<std::string> vec_rotate_to_cmd_strs = {"rotateto" , "ROTATETO", "rotatetoxyz", "ROTATETOXYZ" };

	// TODO: MAKE COMMANDS
	// 1)	MoveTo - COMPLETE
	// 2)	OrientTo - COMPLETE
	// 3)	Follow - TODO: Curvre & Object, Curve: Waypoint / Bezier | Object: distance, offset, following target, following source, max follow speed, acceleration / deceleration range.
	// 4)	Trigger - TODO: Location: trigger an event at the specific location.
	// 5)	CommandGroups - TODO: Serial: execute in order | Parallel: execute simotaneously
	// 6)	Mixed Command Groups - TODO: make mixed command groups
	// 7)	Animation Scripts - TODO: Create animation scripts ( top level command containing command groups which contain commands )
	// 8)	Texture Swaping - TODO: Create texture swapping command for the exam
	// 9)	Texture Blending - TODO: Create texture blending command for the exam
	// 10)	Scale - TODO: Create a scale command for the exam
	

	// CommandTypes: Repeated, Finite
	// Camera needs to be controllable via command ( implement the subsystem.control cmd to account for user input as well )

public:
	sData data;
	std::map<std::string, iCommand*> _map_script_to_cmd;


	static cCommandManager* GetCommandManager();

	void IssueCommand(std::string cmd);
	void IssueCommand(sData& cmd_data);
	void setMediatorPointer(iMediatorInterface* pMediator);

	void CreateCommands();
	iCommand* CreateAnimaScript(cmdType cmd);
	iCommand* CreateCommandGroup(cmdType cmd);
	iCommand* CreateCommand(cmdType cmd);


	virtual sNVData RecieveMessage(sNVData message);
	virtual sData RecieveMessage(sData& data);
};
