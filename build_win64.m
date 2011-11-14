function build_win64()% Compile utils first

mex_suffix = 'mexw64';

debug = true;
if debug,
  debug_str = '-g ';
  debug_lib_suffix = 'd';
else
  debug_str = '';
  debug_lib_suffix = '';
end

flags = debug_str;
opencv_flags = ['-IG:\opencv\opencv\build\install\include\ ' ...
                '-LG:\opencv\opencv\build\install\lib ' ...
                '-lopencv_core232d ', '-lopencv_ml232d '];

compile_and_print(sprintf('mex %s -c mex_utils.cpp', flags));
compile_and_print(sprintf('mex %s -c opencv_matlab_interop.cpp %s', flags, opencv_flags));

rand_forest_funcs = {'rf_delete', 'rf_info', 'rf_load', 'rf_oob', ...
                     'rf_predict', 'rf_save', 'rf_train', 'mex_test'};
for i=1:length(rand_forest_funcs),
    compile_and_print(sprintf('mex %s random_forest/%s.cpp mex_utils.obj opencv_matlab_interop.obj -outdir random_forest %s', ...
                   flags, rand_forest_funcs{i}, opencv_flags));
end

end

function compile_and_print(str)
    fprintf('%s\n',str);
    system(str);
end
