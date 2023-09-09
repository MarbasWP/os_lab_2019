if [ $# -eq 0 ]; then
    echo "Введите аргументы"
    exit 1
fi

sum=0
count=0

for arg in "$@"; do
    sum=$((sum + arg))
    count=$((count + 1))
done

average=$(echo "scale=2; $sum / $count" | bc)
echo "Количество аргументов: $count"
echo "Среднее арифметическое: $average"