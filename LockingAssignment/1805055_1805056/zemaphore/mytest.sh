# g++ test-zem.c zemaphore.c -o test-zem -lpthread
# for i in {1..100}
# do
#     echo "Running test-zem.c"
#     ./test-zem
#     echo
# done
# rm test-zem

g++ test-toggle.c zemaphore.c -o test-toggle -lpthread
# for i in {1..100}
# do
    echo "Test: %i"
    ./test-toggle
# done
rm test-toggle