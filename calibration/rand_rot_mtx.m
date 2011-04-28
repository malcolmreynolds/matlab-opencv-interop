function R = rand_rot_mtx(max_angle)

%first part gets a random val from -1 to 1, second 
degs = max_angle;
v1 = 2*(rand-0.5) * degs * (pi/180);
v2 = 2*(rand-0.5) * degs * (pi/180);
v3 = 2*(rand-0.5) * degs * (pi/180);
R = [cos(v1) sin(v1) 0; -sin(v1) cos(v1) 0; 0 0 1];
R = R * [cos(v2) 0 sin(v2);0 1 0;-sin(v2) 0 cos(v2)];
R = R * [1 0 0;0 cos(v3) sin(v3);0 -sin(v3) cos(v3)];
