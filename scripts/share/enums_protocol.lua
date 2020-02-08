

PTO_C2C_SIGNUP                          = enum_reset()
PTO_C2C_LOGIN                           = enum_next()
PTO_C2C_LOGOUT                          = enum_next()
PTO_C2C_MOVE_TO_POS                     = enum_next()
PTO_C2C_PLAYER_ENTER                    = enum_next()
PTO_C2C_NPC_ENTER                       = enum_next()
PTO_C2C_CHAT                            = enum_next()
PTO_C2C_SAVE_PLAYER_DATABASE            = enum_next()
PTO_C2C_SAVE_ACCOUNT_DATABASE           = enum_next()

PTO_C2S_COMBAT_START                    = enum_next()
PTO_S2C_COMBAT_START                    = enum_next()
PTO_S2C_COMBAT_EXECUTE                  = enum_next()
PTO_C2S_COMBAT_CMD                      = enum_next()
PTO_S2C_SYNC_PROPS                      = enum_next()

PTO_C2S_NPC_DIALOG                      = enum_next()
PTO_S2C_NPC_DIALOG                      = enum_next()

PTO_C2S_DOSTRING                        = enum_next()
PTO_C2S_PLAYER_DOSTRING                 = enum_next()
PTO_C2S_ACTOR_DOSTRING                  = enum_next()
PTO_C2S_CREATE_PLAYER                   = enum_next()

PTO_C2S_CLICK_NPC                         = enum_next()
PTO_S2C_CLICK_NPC                         = enum_next()

PTO_C2S_TEAM_CREATE                         = enum_next()

PTO_S2C_TEAM_CREATE                         = enum_next()

PTO_C2S_TEAM_ENTER_TEAM                        = enum_next()
PTO_C2S_TEAM_LEAVE_TEAM                         = enum_next()
PTO_C2S_TEAM_CHANGE_LEADER                         = enum_next()

PTO_C2S_TEAM_DISMISS                         = enum_next()
PTO_S2C_TEAM_DISMISS = enum_next()

PTO_C2S_TEAM_ADD_MEMBER                         = enum_next()
PTO_S2C_TEAM_ADD_MEMBER                         = enum_next()
PTO_C2S_TEAM_REMOVE_MEMBER                        = enum_next()
PTO_S2C_TEAM_REMOVE_MEMBER                        = enum_next()

PTO_C2S_FETCH_TEAM = enum_next()
PTO_S2C_FETCH_TEAM = enum_next()

PTO_C2S_COMBAT_LEAVE_BATTLE = enum_next()
PTO_S2C_COMBAT_LEAVE_BATTLE = enum_next()


PTO_C2S_COMBAT_END_BATTLE = enum_next()
PTO_S2C_COMBAT_END_BATTLE = enum_next()

PTO_C2S_CREATE_SUMMON = enum_next()
PTO_S2C_CREATE_SUMMON_RESP = enum_next()

PTO_C2S_FETCH_SUMMON = enum_next()
PTO_S2C_FETCH_SUMMON_RESP = enum_next()


PTO_C2S_GM = enum_next()
PTO_S2C_GM = enum_next()