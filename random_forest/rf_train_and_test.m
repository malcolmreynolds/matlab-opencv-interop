function results = rf_train_and_test(train_x, train_y, test_x, test_y, training_params)
%train a forest on the training set, run it on the test set and
%return the sum of squared errors. Deletes the forest after it is used.
forest = rf_train(train_x, train_y, training_params);

[pred_mean pred_var] = rf_predict(forest, test_x);
results.pred_mean = pred_mean;
results.pred_var = pred_var;
results.sse = sum((pred_mean - test_y).^2);
results.mse = results.sse / length(test_y);
results.tree_info = rf_info(forest);

rf_delete(forest);
