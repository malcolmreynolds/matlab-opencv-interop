function fc_delete(forest_collection)
	% fc_delete(forest_collection)
	%  Deallocate a collection of random forests
	num_forests = length(forest_collection.forests);
	
	for i=1:num_forests,
		rf_delete(forest_collection.forests{i});
	end
end
	
	
