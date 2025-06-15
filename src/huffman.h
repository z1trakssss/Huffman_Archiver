#pragma once
#include <string>
#include <memory>
#include <map>

/**
 * @file huffman.h
 * @brief Заголовочный файл для класса HuffmanArchiver и структуры Node, используемых в кодировании Хаффмана.
 *
 * Этот файл содержит объявления класса HuffmanArchiver, реализующего алгоритм
 * кодирования Хаффмана для сжатия и распаковки файлов, а также структуры Node,
 * представляющей узлы дерева Хаффмана.
 */

/**
 * @struct Node
 * @brief Представляет узел дерева Хаффмана, используемого для кодирования и декодирования.
 *
 * Каждый узел хранит символ, его частоту и указатели на левый и правый дочерние узлы.
 * Листовые узлы содержат символы и их частоты, а внутренние узлы объединяют
 * частоты своих дочерних узлов.
 */
struct Node {
    /** @brief Символ, хранимый в узле (действителен для листовых узлов). */
    unsigned char symbol;

    /** @brief Частота символа или сумма частот дочерних узлов. */
    uint64_t freq;

    /** @brief Указатель на левый дочерний узел (nullptr для листовых узлов). */
    std::shared_ptr<Node> left;

    /** @brief Указатель на правый дочерний узел (nullptr для листовых узлов). */
    std::shared_ptr<Node> right;

    /**
     * @brief Конструктор листового узла с символом и частотой.
     * @param s Символ, который будет храниться в узле.
     * @param f Частота символа.
     */
    Node(unsigned char s, uint64_t f);

    /**
     * @brief Конструктор внутреннего узла с двумя дочерними узлами.
     * @param l Указатель на левый дочерний узел.
     * @param r Указатель на правый дочерний узел.
     */
    Node(std::shared_ptr<Node> l, std::shared_ptr<Node> r);
};

/**
 * @class HuffmanArchiver
 * @brief Реализует кодирование Хаффмана для сжатия и распаковки файлов.
 *
 * Этот класс предоставляет методы для сжатия и распаковки файлов с использованием
 * алгоритма кодирования Хаффмана. Он создает таблицу частот, строит дерево Хаффмана
 * и генерирует коды Хаффмана для эффективного кодирования. Класс также поддерживает
 * вывод таблицы частот во время распаковки для отладки или анализа.
 */
class HuffmanArchiver {
private:
    /** 
     * @brief Таблица, отображающая символы на их частоты во входном файле.
     */
    std::map<unsigned char, uint64_t> freq_table;

    /** 
     * @brief Таблица, отображающая символы на их коды Хаффмана.
     */
    std::map<unsigned char, std::string> huffman_codes;

    /** 
     * @brief Указатель на корень дерева Хаффмана.
     */
    std::shared_ptr<Node> root;

    /**
     * @brief Создает таблицу частот из входного файла.
     * @param input_file Путь к входному файлу.
     * @throws std::runtime_error Если входной файл не удается открыть.
     */
    void buildFrequencyTable(const std::string& input_file);

    /**
     * @brief Строит дерево Хаффмана на основе таблицы частот.
     */
    void buildHuffmanTree();

    /**
     * @brief Генерирует коды Хаффмана, обходя дерево Хаффмана.
     * @param node Указатель на текущий узел дерева.
     * @param code Текущий код, формируемый (строка из '0' и '1').
     */
    void buildHuffmanCodes(const std::shared_ptr<Node>& node, const std::string& code);

    /**
     * @brief Записывает таблицу частот в выходной поток.
     * @param out Выходной поток для записи таблицы частот.
     */
    void writeFrequencyTable(std::ofstream& out);

    /**
     * @brief Считывает таблицу частот из входного потока.
     * @param in Входной поток для чтения таблицы частот.
     * @throws std::runtime_error Если таблица частот повреждена или не может быть прочитана.
     */
    void readFrequencyTable(std::ifstream& in);

public:
    /**
     * @brief Сжимает входной файл с использованием кодирования Хаффмана.
     * @param input_file Путь к входному файлу для сжатия.
     * @param output_file Путь к выходному сжатому файлу.
     * @throws std::runtime_error Если файлы не удается открыть, входной файл пуст или сжатие не удалось.
     */
    void compress(const std::string& input_file, const std::string& output_file);

    /**
     * @brief Распаковывает архив, закодированный алгоритмом Хаффмана.
     * @param input_file Путь к сжатому входному файлу.
     * @param output_file Путь к выходному распакованному файлу.
     * @param write_freq Если true, записывает таблицу частот в файл с суффиксом "_freq.txt".
     * @throws std::runtime_error Если файлы не удается открыть, архив пуст или поврежден, или распаковка не удалась.
     */
    void decompress(const std::string& input_file, const std::string& output_file, bool write_freq = false);

    /**
     * @brief Возвращает таблицу кодов Хаффмана (только для тестирования).
     * @return Константная ссылка на карту символов и их кодов Хаффмана.
     */
    const std::map<unsigned char, std::string>& getHuffmanCodes() const { return huffman_codes; }
};