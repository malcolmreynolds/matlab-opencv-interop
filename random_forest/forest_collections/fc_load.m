function forest_collection = fc_load(load_path)
	% forest_collection = fc_load(load_path)
	%  Loads a forest collection from a folder. See fc_save for 
	%  why the method is a bit whacky.
	
	fprintf('loading from %s\n', load_path);
	if ~exist(load_path),
		error('cannot load from %s, doesn''t exist.', load_path);
	end
	if load_path(end) ~= filesep,
		load_path = [load_path filesep];
	end
		
	forest_collection = load([load_path 'forest_collection.mat']);
	forest_collection = forest_collection.forest_collection;
	
	num_forests = length(forest_collection.forests);
	
	fprintf('Loaded central collection structure, restoring %d forests...\n', num_forests);
	
	for i=1:num_forests,
		% The struct should contain filenames, overwrite those with the actual forest pointer
		forest_collection.forests{i} = rf_load([load_path forest_collection.forests{i}]);
		fprintf('done %d\n', i);
	end
	
	fprintf('successfully loaded %d forests\n', num_forests);
end