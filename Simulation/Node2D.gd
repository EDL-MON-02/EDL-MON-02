extends Node2D

var dust_particle_scene = preload("res://dust.tscn")
var spawn_area_rect = Rect2(Vector2(100, 100), Vector2(800, 600)) # Define your spawn area

func _ready():
    spawn_initial_dust_particles()

func spawn_initial_dust_particles():
    var num_particles = 100  # Adjust the number of initial particles as needed
    for i in range(num_particles):
        spawn_dust_particle()

func spawn_dust_particle():
    var dust_particle = dust_particle_scene.instance()
    dust_particle.position = generate_random_position()
    add_child(dust_particle)

func generate_random_position():
    return Vector2(
        rand_range(spawn_area_rect.position.x, spawn_area_rect.end.x),
        rand_range(spawn_area_rect.position.y, spawn_area_rect.end.y)
    )
