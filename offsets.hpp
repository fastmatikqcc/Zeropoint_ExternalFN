enum bone : int {
	Head = 110,
	Neck = 67,
	Chest = 66,
	Pelvis = 2,
	LShoulder = 9,
	LElbow = 10,
	LHand = 11,
	RShoulder = 38,
	RElbow = 39,
	RHand = 40,
	LHip = 71,
	LKnee = 72,
	LFoot = 73,
	RHip = 78,
	RKnee = 79,
	RFoot = 82,
	Root = 0
};

enum offset {
	uworld = 0x12D17A08,
	game_instance = 0x1F0,
	game_state = 0x178,
	local_player = 0x38,
	player_controller = 0x30,
	acknowledged_pawn = 0x350,
	aactor = 0xa0,
	skeletal_mesh = 0x328,
	player_state = 0x2b0,
	root_component = 0x1B0,
	velocity = 0x168,
	relative_location = 0x120,
	relative_rotation = 0x130,
	bone_array = 0x580,
	bone_cache = 0x5B8,
	OwningWorld = 0xc0,
	current_weapon = 0x9d8,
	weapon_data = 0x568,
	tier = 0x9b,
	team_index = 0x1239,
	player_array = 0x2c0,
	pawn_private = 0x320,
	component_to_world = 0x1c0,
	TargetedFortPawn = 0x18e8, //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AFortPlayerController&member=TargetedFortPawn
	FOVMinimum = 0x2750, // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AFortPlayerController&member=FOVMinimum
	FOVMaximum = 0x2754, // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AFortPlayerController&member=FOVMaximum
	OnSetFirstPersonCameraa = 0x10b8, // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AFortPlayerController&member=OnSetFirstPersonCamera
};

#define FortPTR uintptr_t

#define CURRENT_CLASS reinterpret_cast<uintptr_t>(this)
#define DECLARE_MEMBER(type, name, offset) type name() { return read<type>(CURRENT_CLASS + offset); }
#define DECLARE_MEMBER_DIRECT(type, name, base, offset) type name() { read<type>(base + offset); }
#define DECLARE_MEMBER_BITFIELD(type, name, offset, index) type name() { read<type>(CURRENT_CLASS + offset) & (1 << index); }
#define APPLY_MEMBER(type, name, offset) void name( type val ) { write<type>(CURRENT_CLASS + offset, val); }
#define APPLY_MEMBER_BITFIELD(type, name, offset, index) void name( type val ) { write(CURRENT_CLASS + offset, |= val << index); }




