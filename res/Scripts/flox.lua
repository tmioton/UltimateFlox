flox.flock_size = 1024
flox.world_bound = 450.0
flox.width = 800
flox.height = 450

--provided:
--  frame_count
--  delta
--  fps

--function OnFrameGroupEnd()
--    print("Average framerate for last " .. frame_count .. " frames: " .. fps .. " fps | " .. 1.0 / fps .. " spf")
--end

--function OnFrameStart()
--    if (delta > (1 / 60)) then
--        print("Last frame took longer than average.")
--    end
--end
