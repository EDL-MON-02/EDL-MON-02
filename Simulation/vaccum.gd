extends KinematicBody2D



var collision_c = false
var collision_r = false
var collision_l = false

var fast_forward = 1
var timer = 0
var timer2 = 0
var timer3 = 0
var speed = 6500*fast_forward
var speed3 = 8000

var mode = 4
# 0 = s-shape
# 1 = circle 
# 2 = random
# 4 = map based

var s_turn = 1


export(float) var rotate_speed = 5.0
export(float) var radius = 100
var angle = 0
var i = 1
var direction_reversed = false
var j = 1


var wall_detected = false

var wall_normal = Vector2.ZERO


# 0 = unexplored
# 1 = cleaned
# -1 = obstacle

var grid = []

export(float) var tile_size = 64


var i0 = 0

var rng = RandomNumberGenerator.new()

func _ready():
    for x in range(10): # Adjust grid size as needed
        var row = []
        for y in range(10): # Adjust grid size as needed
            row.append(0) # 1 represents unexplored area
        grid.append(row)
	
func _process(delta):
	if Input.is_action_just_pressed("random"):
		mode = 2
	if Input.is_action_just_pressed("c"):
		mode = 1
	if Input.is_action_just_pressed("s"):
		mode = 0
	if Input.is_action_just_pressed("ui_up"):
		mode = 3
	
	
	
	if mode == 4:
		
		if (i0%60 == 0):
			print(grid)
			print("--------------------------------------------------")
		var forward_vector = Vector2(0, -1).rotated(rotation)
		move_based_on_grid(forward_vector, delta)
			
		
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	if mode == 0:
		var forward_vector = Vector2(0, -1).rotated(rotation) 
		#timer += delta
		if collision_r&&collision_l :
			rotate(deg2rad(180))
			wait()
			collision_r = false
			collision_l = false
			
		elif collision_r == true :
			#s_turn = -1 
			rotate(deg2rad(-90))
			wait()
			collision_r = false
			
		elif collision_l == true :
			#s_turn = 1
			rotate(deg2rad(90))
			wait()
			collision_l = false
			
		if collision_c == true :
			print ("rotating")
			rotate(s_turn*deg2rad(90))
			
			slow_move(delta , collision_c  , collision_r ,collision_l,timer2 )
			
			rotate(s_turn*deg2rad(90))
			collision_c = false
			s_turn = -s_turn
			

		forward_vector = Vector2(0, -1).rotated(rotation) 
		var motion = forward_vector * speed * delta
		move_and_slide(motion)
			
	if mode == 1:
	    if not is_on_wall():
	        # If not on the wall, increment the spiral parameter
	        i = i + delta / 10
	        # Reset the direction reversed flag
	        direction_reversed = false
	    else:
	        # Decrease the spiral parameter if on the wall
	        i = i - delta

	        # If on the wall and direction hasn't been reversed yet
	        if not direction_reversed:
	            # Reverse the direction control variable j
	            j = -j
	            # Set the direction reversed flag to true
	            direction_reversed = true

	    # Calculate the spiral movement
	    angle = fmod(angle + (rotate_speed / (i) * delta), 2 * PI)
	    var offset = j * Vector2(sin(angle), cos(angle)) * (radius + i)
	    rotation = rotation - deg2rad(atan(sin(angle) / cos(angle)))
	    
	    # Move the vacuum cleaner
	    move_and_slide(offset)
			
	if mode == 2:
		var forward_vector = Vector2(0, -1).rotated(rotation) 
		timer3 += delta
		if (collision_r || collision_l || collision_c)  :
			timer3 += delta
			if timer3 > 0.01 :
				
				var rot = rng.randf_range(PI*0.3, 1.8*PI)
				rotate(rot)
				wait()
				collision_r = false
				collision_l = false
				collision_c = false
				timer3 = 0
				
	
			
		else :
			
			var motion = forward_vector * speed3 * delta
			move_and_slide(motion)
			
	if mode == 3:
		var dum = 1
		
		var forward_vector = Vector2(0, -1).rotated(rotation) 
		timer3 += delta
		if (collision_r or collision_l or collision_c or timer3 > rng.randf_range(rng.randf_range(1,10) ,rng.randf_range(30,120))  )  :
			
			var j = rng.randi_range(0, 1) * 2 - 1
			var rot = j*dum*PI/2
			if collision_r :
				rot = -1*PI/2
			if collision_l :
				rot = 1*PI/2
			
			
			dum = - dum
			rotate(rot)
			wait()
			collision_r = false
			collision_l = false
			collision_c = false
			timer3 = 0
			

				
				
	
			
		var motion = forward_vector * speed3 * delta
		move_and_slide(motion)
			
		

		
		
		

	
	
func wait():
	var time = 0
	while time < 1000000 :
		time += time + 0.1 
		






func slow_move(delta , collision_c  , collision_r ,collision_l,timer2 ):
	
	
	while timer2 <  1:
		if collision_c && collision_l && collision_r:
			break	
		var forward_vect = Vector2(0, -1).rotated(rotation) 	
		var smotion = forward_vect * speed * delta
		move_and_slide(smotion)
		timer2 += delta
	timer2 = 0
		
	
	

func move_based_on_grid(forward_vector, delta):
    var motion = forward_vector * speed * delta
    var next_position = position + motion
    var grid_x = int(next_position.x / tile_size)
    var grid_y = int(next_position.y / tile_size)
    
    if grid_x >= 0 and grid_x < grid.size() and grid_y >= 0 and grid_y < grid[0].size():
        if grid[grid_x][grid_y] == 0:
            # Move to the next position if it's cleaned
            move_and_slide(motion)
        else:
            # Rotate randomly if the next position is unexplored or obstacle
            rotate(deg2rad(rand_range(-90, 90)))
    else:
        # Rotate randomly if the next position is out of bounds
        rotate(deg2rad(rand_range(-90, 90)))






func _on_sc_body_entered(body):
	if body is TileMap:
		collision_c = true
		print("front collision")
	


func _on_sr_body_entered(body):
	if body is TileMap:
		collision_r = true
		#print("right collision")


func _on_sl_body_entered(body):
	if body is TileMap:
		collision_l = true
		#print("left collision")


func _on_sc_body_exited(body):
	if body is TileMap:
		collision_c = false
		print("front collision over")


func _on_sr_body_exited(body):
	if body is TileMap:
		collision_r = false
		#print("right collision over")


func _on_sl_body_exited(body):
	if body is TileMap:
		collision_l = false
		#print("left collision over")
