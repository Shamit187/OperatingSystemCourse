echo "TESTSET: Running Testcases with reader preference"
g++ test-reader-pref.cpp rwlock-reader-pref.cpp -o rwlock-reader-pref -lpthread

for i in {5..10}
do
    for j in {1..10}
    do
    out=`./rwlock-reader-pref $i $j`
    echo $i "," $j "=" $out
    done
done

echo "TESTSET: Running Testcases with writer preference"
g++ test-writer-pref.cpp rwlock-writer-pref.cpp -o rwlock-writer-pref -lpthread

for i in {1..10}
do
    for j in {1..10}
    do
    out=`./rwlock-writer-pref $i $j`
    echo $i "," $j "=" $out
    done
done

rm rwlock-reader-pref
rm rwlock-writer-pref