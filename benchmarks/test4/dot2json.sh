dirlist=`ls .`
for dir in $dirlist
do
    if [ -d $dir ]; then
        cd $dir
        # ls *.dot
        filelist=`ls *.dot`
        for file in $filelist
        do
            count=`ls $file | grep mapped | wc -l`
            if [ $count == "0" ]; then
                # echo $file
                name=`ls $file | cut -f 1 -d '.'`
                dot -Tdot_json $file -o $name.json
                echo "$file -> $name.json" 
            fi
        done
        cd ..
    fi
done
