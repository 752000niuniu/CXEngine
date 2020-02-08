stub[PTO_C2S_GM] = function(msg)
    if msg.type == 'exit_server' then
        os.exit(0)
    else
        net_send_message_to_all_players(PTO_S2C_GM, cjson.encode(msg))
    end
end