echo "Test #1: etalon calculations"
../dtools/dtf test.ssm u.dat y_.dat
diff -u y.dat y_.dat
../dtools/dtf test_2x2.ssm u2.dat y2_.dat
diff -u y2.dat y2_.dat
echo "Test #2: TF and SSM equality"
../dtools/dtf plant.tf u_step.dat y_tf.dat
../dtools/dtf plant.ssm u_step.dat y_ssm.dat
diff -u y_tf.dat y_ssm.dat
