function fc_save(forest_collection, save_location)
	% fc_save(forest_collection, save_location)
	%  Saves forests, which must be a struct with a field 'forests' which is a cell array
	%  of pointers as returned by opencv. Because of this, needs to save to a
	%  directory rather than a file.
	
	num_forests = length(forest_collection.forests);
	
	fprintf('saving %d forests to %s...\n', num_forests, save_location);
	
	% Add a trailing slash if needed, then check if it already exists
	if save_location(end) ~= filesep,
		save_location = [save_location filesep];
	end
	if exist(save_location) ~= 0,
		error('Save location %s already exists!', save_location);
	end
	
	[success, message, message_id] = mkdir(save_location);
	if success ~= 1,
		error('couldn''t create folder to save in: %s %s', message, message_id);
	end
	
	% Save each forest one by one
	for i=1:num_forests,
		fname = sprintf('forest_%d.frst', i)
		rf_save(forest_collection.forests{i}, ...
				sprintf('%s%s', save_location, fname));
		% We overwrite the pointer with the filename so that when we
		% save out and restore the forest_collection struct (in standard 
		% matlab fashion) we then have a cell array of strings which we know
		% we need to load the opencv forest from. It'll be great, trust.
		forest_collection.forests{i} = fname;
	end
	
	% Now save the main structure - this can have all sorts of metadata, save
	% using standard matlab serialization to support this.
	save([save_location 'forest_collection.mat'], 'forest_collection');
end
		
	
