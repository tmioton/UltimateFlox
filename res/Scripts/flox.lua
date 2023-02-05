flox.flock_size = 1024
flox.world_bound = 500.0
flox.width = 800
flox.height = 450

--frame_total = 0.0
--frame_count = 0
--total_average = 0.0
--total_average_count = 0
--function OnFrameStart(delta)
--    frame_count = frame_count + 1
--    frame_total = frame_total + delta
--    if (frame_count >= 60) then
--        print("Average framerate for last", frame_count, "frames:", 1 / (frame_total / frame_count))
--        total_average = total_average + frame_total / frame_count
--        total_average_count = total_average_count + 1
--        frame_count = 0
--        frame_total = 0
--    end
--end


--function OnExit()
--    print("Average framerate for program:", 1 / (total_average / total_average_count))
--end
