function [best_params stats] = rf_train_with_param_search(training_x, training_y, search_ranges)
	% Given some x and y to train on, perform a parameter search to determine what some good parameters
	% are for this data. Uses out-of-bag (oob) error currently, could be modified to do cross validation?
	
	% need to extract this as the default heuristic for setting the num_active_vars is square root of number
	% of variables
	num_vars = size(training_x,2);
	sqrt_num_vars = sqrt(num_vars);
	default_options = {{'max_trees_rng', [5 10 25 50 100 200]}, ...
	                   {'max_depth_rng', [5 10 15 20 25 30]}, ...
	                   {'regression_accuracy_rng', [10 0.1 0.001]}, ...
	                   {'num_active_vars_rng', ceil(sqrt_num_vars*[sqrt_num_vars 3 2 1 0.5])}};
	
	% If no options supplied, give some sensible defaults					
	if nargin < 3,
		search_ranges = struct();
	end
	for i=1:length(default_options),
		name = default_options{i}{1};
		if ~isfield(search_ranges, name),
			search_ranges = setfield(search_ranges, name, default_options{i}{2});
		end
	end
	
	% Trim the num_vars list in case it specified anything higher than the number of variables
	valid_num_vars_mask = search_ranges.num_active_vars_rng <= num_vars;
	search_ranges.num_active_vars_rng = search_ranges.num_active_vars_rng(valid_num_vars_mask);
	
	% Prepare the output array stuff
	stats.oob_error = zeros(length(search_ranges.max_trees_rng), ...
	                        length(search_ranges.max_depth_rng), ...
	                        length(search_ranges.regression_accuracy_rng), ...
	                        length(search_ranges.num_active_vars_rng));
	stats.time = zeros(length(search_ranges.max_trees_rng), ...
					   length(search_ranges.max_depth_rng), ...
					   length(search_ranges.regression_accuracy_rng), ...
					   length(search_ranges.num_active_vars_rng));
	stats.num_trees = zeros(length(search_ranges.max_trees_rng), ...
							length(search_ranges.max_depth_rng), ...
							length(search_ranges.regression_accuracy_rng), ...
							length(search_ranges.num_active_vars_rng));
	stats.search_ranges = search_ranges;
	
	fprintf('running exhaustive search...')
	
	min_oob_error = Inf;
	best_params = -1;
	
	opts = rf_good_params()
	for w=1:length(search_ranges.max_trees_rng),
		opts.max_tree_count = int32(search_ranges.max_trees_rng(w));
		for x=1:length(search_ranges.max_depth_rng), 
			opts.max_depth = int32(search_ranges.max_depth_rng(x));
			for y=1:length(search_ranges.regression_accuracy_rng),
				opts.regression_accuracy = search_ranges.regression_accuracy_rng(y);
				for z=1:length(search_ranges.num_active_vars_rng),
					opts.num_active_vars = int32(search_ranges.num_active_vars_rng(z));
					
					% Train the forest, record the stats, and delete
					[forest stats.time(w,x,y,z)] = rf_train(training_x, training_y, opts);
					forest_info = rf_info(forest);
					stats.oob(w,x,y,z) = forest_info.oob_error;
					stats.num_trees(w,x,y,z) = forest_info.num_trees;
					rf_delete(forest);
					
					% If we have achieved the smallest oob error so far, then store these parameters and
					% the value.
					if forest_info.oob_error < min_oob_error,
						best_params = opts;
						min_oob_error = forest_info.oob_error;
					end	
				end
			end
			fprintf('dont max depth = %d\n', opts.max_depth);
		end
		fprintf('done max trees = %d\n', opts.max_tree_count);
	end
	
end
	
					
					
		