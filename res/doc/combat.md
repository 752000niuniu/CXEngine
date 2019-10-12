进入战斗, 己方组5人 对方组5人 
actor类型: 玩家 BB 电脑 其他玩家

战斗指令: 物理攻击, 法术攻击(单法 群法 封印), 吃药, 特技, 加buff, 加debuff

1. 每回合倒计时30s
2. 每个玩家发出自己的战斗指令到服务器, 服务器接收每个玩家发出的战斗指令, 排序, 伤害计算. 下发. 计算回合总时间,下发.
3. 服务器倒计时回合时间, 时间到,进入下一个回合倒计时.
4. 客户端收到回合时间,还有完整的战斗指令后, 本地进行动画播放. 播放后,等待服务器下一个回合倒计时.
5. 服务器客户端之间的延时


客户端战斗表现设计

战斗环境：
PVP 最多5V5  每个Actor可以召唤自己的宝宝
PVE 最多5V10  对方是NPC 或者 普通Actor
战斗分两组 自己组（SelfActors）  对方组（EnemyActors）
回合制战斗机制：
1. 回合开始，倒计时等待战斗指令的输入
2. 战斗指令进行排序
3. 上一回合的战斗指令处理
4. 本回合战斗指令处理
5. 回合结束，进入下一个回合

#战斗指令

类型：
施法技能 ：物理施法，法术施法，特技施法，辅助施法
普通攻击
防御
召唤
捕捉

战斗指令队列
回合计数
              


master 攻击actor
target 攻击目标actor

hp +-	hp为0表示死亡 
sp +-	sp不足 不能释放特技
mp +-	mp不足 不能释放技能

攻击力  
防御力
灵力
速度
躲避

1. 攻击类

攻击类动画表现为 master移动到target进行攻击 

状态机阶段:
				非防御						防御
[普通攻击] 	   受击动作+普通受击效果		防御动作+防御效果							
[攻击类技能]   受击动作+技能受击效果		防御动作+技能受击效果  

				是否连击 是否暴击 是否吸血 
普通攻击 			Y 		Y		Y
攻击类技能					Y 		Y 



攻击类技能也包括暴击

普通攻击时, 如果对方处于防御状态, 伤害减半
普通攻击时, 有可能出现连击, 暴击, 吸血, 被反震, 被反击, 击飞目标, 击倒目标

攻击技能流程:

攻击者	待战 -> 跑去 -> 攻击       -> 跑回 ->  待战
受击者	待战              受击                    待战



2. 法术类

3. 法术攻击指令


技能表里的技能都是瞬时的 当前回合释放后处理
技能释放后,可能产生buff到target的身上,
每回合会对buff做统一处理 例如表现效果展示 , 计算血量 等等

每回合读完所有战斗指令 根据actor速度模拟战斗指令释放,计算结果
回合开始后,对战斗指令进行播放
回合结束, 只用退出播放就行了.


横扫千军 分解成 
	1. 横扫千军技能 2. 横扫千军buff 
	buff表现是玩家脚下踩风火轮 防御力减半 第二回合不能行动, 在横扫千军释放完把buff给加上, 
	buff被加上后 , 玩家脚踩风火轮, 第二回合 buff持续生效, 第二回合结束 buff更新 判断buff无效

后发制人 分解成
	1. 后发技能释放完 增加一个buff 
	2. 后发buff在第二个回合开始buff处理的时候, 控制actor进行后发2技能attack攻击, 然后buff被移除掉

横扫伤害计算
	释放横扫的回合前, 可以提前算出 横扫对目标造成的伤害 以及横扫自己消耗的hp


cmd = {
    master,
    target,
    skill_id
}

atk_skill = {
    type = 'atk',
    atk_anim = '',
    combo,
    group_kill,
    buff_id,
    base_damage,
    is_suck_blood,
    is_critical_atk,
}

cast_skill = {
    type = 'cast',
    cast_anim,
    buff_id
}

buff = {
    turn_count,
    curr_turn,
    buff_state_anim,
    buff_props
}


播放攻击指令

待战-> 跑去-> 攻击 -> 

SkillOnStart

SkillOnCombo

SkillOnGroupKill

SkillOnEnd

读取攻击技能id, 是否有combo , 是否是群秒, 是否产生暴击, 是否吸血, 攻击效果, 是否会添加buff
添加buff的话, 添加后 调用buffOnStart

-> 跑回 -> 结束


播放施法指令

待战-> Cast动作->
SkillOnStart 

SkillOnCombo

SkillOnGroupKill

SkillOnEnd

-> 待战