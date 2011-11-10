function opts = rf_good_params(num_variables)
	% Returns a set of parameters which hopefully work well.
	opts.max_tree_count = int32(200);
	opts.regression_accuracy = 1e-13;
	opts.forest_accuracy = 1e-13;
end