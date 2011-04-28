function posit_test(intrinsics, extrinsics)
% all this junk ported from http://opencv.willowgarage.com/wiki/Posit

%size - corners of a cube
s = 10;
model_points = [0 0 0; ...
                s 0 0; ...
                0 s 0; ...
                0 0 s]';
num_points = 4;

k = intrinsics;
r = extrinsics.r;
t = extrinsics.t;

%convert 3d points to homogeneous and do full projective transform
model_points_transformed = k * [r,t] * [model_points; ones(1, size(model_points,2))];
%divide to convert from 2D homogeneous to 2D cartesian
image_points = model_points_transformed(1:2,:) ./ repmat(model_points_transformed(3,:), 2, 1)

focal_length = (intrinsics(1,1) + intrinsics(2,2)) / 2;

fprintf('calling POSIT with focal_length = %f\nintrinsics = \n', focal_length);
fprintf('%f %f %f\n', k');
fprintf('extrinsics r = \n');
fprintf('%f %f %f\n', r');
fprintf('extrinsics t = %f %f %f\n', t);

[r t] = posit(model_points, image_points, focal_length, int32(500))

                                                  