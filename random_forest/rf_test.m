function rf_test
	
	% feature data
	[x y] = meshgrid(linspace(-5,5,50), linspace(-5,5,50));
	X = [x(:), y(:)]; 
	
	% labels
	y = 2*X(:,1).^2 - 0.5*X(:,2).^3;
	
	forest = rf_train(X,y);
		
	forest_path = [datestr(now, 'YYYYmmDDHHMM') '_forest.frst'];
	rf_save(forest, forest_path);
	fprintf('saved\n');
	
	rf_delete(forest);
	forest = -1;
	fprintf('deleted\n');
	
	forest = rf_load(forest_path);
	fprintf('loaded\n');
	
	predictions = rf_predict(forest, X);
	fprintf('done ting\n');
	
