function posit_test(intrinsics, extrinsics)
% all this junk ported from http://opencv.willowgarage.com/wiki/Posit

%size - corners of a cube
s = 10;
model_points = [0 0 0; ...
                s 0 0; ...
                0 s 0; ...
                0 0 s; ...
                ]';
                %s s 0; ...
                %s 0 s; ...
                %0 s s; ...
                %s s s]';
num_points = size(model_points, 2);

%extract focal lengths. Since we can only give one to the posit function, use mean.. is this smart? 
focal_x = intrinsics(1,1);
focal_y = intrinsics(2,2);
focal_length = (focal_x + focal_y) / 2;
r = extrinsics.r;
t = extrinsics.t;

%convert 3d points to homogeneous and do full projective transform
model_points_transformed = [r,t] * [model_points; ones(1, num_points)];
%divide to convert from 2D homogeneous to 2D cartesian
focal_length_mtx = [intrinsics(1,1) * ones(1, num_points); ...
                    intrinsics(2,2) * ones(1, num_points)];
image_points = focal_length_mtx .* model_points_transformed(1:2,:) ./ repmat(model_points_transformed(3,:), 2, 1)





%fprintf('calling POSIT with focal_x = %f, focal_y = %f, focal_length=%f\n', ...
%        focal_x, focal_y, focal_length);
fprintf('extrinsics r = \n');
fprintf('%f %f %f\n', r');
fprintf('extrinsics t = %f %f %f\n\n', t);

[r t] = posit(model_points, image_points, focal_length, int32(20));

fprintf('computed r =\n');
fprintf('%f %f %f\n', r');
fprintf('computed t = %f %f %f\n', t);

                                                  