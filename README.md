***** 193541T AGDEV A1 *****

Game Controls:

Esc - Exit programme
Left Mouse Button (LMB) - Shoot
Right Mouse Button (RMB) - Reduce Field Of View (FOV)
F1 - Toggle fullscreen
F2 - Change polygon mode

WASD - Move cam when cam is detached from player/Move cam and player when cam is attached to player
Space - Jump
R - Reload currently selected gun
C - Show/Hide colliders
B - Attach/Detach cam
N - Show/Hide regions
M - Show/Hide viewing frustum
U - Rotate viewing frustum anticlockwise
O - Rotate viewing frustum clockwise
IJKL - Move player when cam is detached from player
1 - Select inv[0]
2 - Select inv[1]
3 - Select inv[2]
4 - Select inv[3]
5 - Select inv[4]
6 - Toggle shldUpdateEntityManager
7 - Toggle shldRenderEntityManager

Non-Game Controls:

Left Mouse Button (LMB) - select different options

Features:

Instancing
- Can translate, rotate and scale instanced group
- Can have different transforms, color and texture per instance

Demo of frame rate
- Elements in the scene are rendered to show a visual output of the effects
- Text on screen shows prog performance stats

Advanced Collision Detection (ACD)
- Raycasting (can collide with thin obj)
- Fused with Quadtree for higher efficiency
- Cam collision with terrain (using barycentric interpolation)
- Rendering of colliders

LOD
- Diff mesh(es)/model(s) and texture(s)
- 3 diff detail lvls (High, Medium, Low)
- Allows for developer(s) to choose to not show the mesh(es)/model(s) at all
- Merged with instancing

Scene Graph
- Merged with Spatial Partitioning in Quadtree

Spatial Partitioning
- Merged with Scene Graph in Quadtree
- Rendering of Quadtree

Frustum Culling
- Merged with Quadtree
- Can render out viewing frustum

New others
- Easing
- Rotation using quaternions
- Generic class for Obj Pooling
- Better abstraction for most parts of the code

Old others
- Walking
- Jumping
- Shooting weapons
- Changing weapons
- Diff weapon attributes
- Inventory
- Night vision (combined with scoped mode for sniper)
- Scoped mode (for sniper)
- Terrain with normals
- Sprite Animation
- Skybox with bounds checking
- 2D audio
- 3D audio

No warnings (except for the ones from the external libraries)
No memory leaks