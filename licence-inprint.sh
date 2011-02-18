for i in src/*.c # or whatever other pattern...
do
  if ! grep -q Copyright $i
  then
    cat licence-head.txt $i >$i.new && mv $i.new $i
  fi
done

for i in src/*.h # or whatever other pattern...
do
  if ! grep -q Copyright $i
  then
    cat licence-head.txt $i >$i.new && mv $i.new $i
  fi
done
