import json
import matplotlib.pyplot as plt
import numpy as np

def plot_data(data_type):
    # 假设文件路径已经正确指向了你的JSON文件
    json_file_1 = 'dumas_with_heuristic.json'
    json_file_2 = 'dumas_origin.json'
    
    # 加载两个JSON文件中的数据
    with open(json_file_1, 'r') as f1, open(json_file_2, 'r') as f2:
        data_with_heuristic = json.load(f1)
        data_origin = json.load(f2)
    
    # 筛选出n20w20的数据集进行比较
    filtered_data_with_heuristic = [item for item in data_with_heuristic if data_type in item['file']]
    filtered_data_origin = [item for item in data_origin if data_type in item['file']]
    
    filtered_data_with_heuristic.sort(key=lambda d: d['file'])
    filtered_data_origin.sort(key=lambda d: d['file'])
    
    # 提取运行时间和标签数量
    runtimes_with_heuristic = []
    labels_with_heuristic = []
    runtimes_origin = []
    labels_origin = []
    
    for i in range(5):
        if (filtered_data_with_heuristic[i]['return_code'] == 0 and filtered_data_origin[i]['return_code'] == 0):
            runtimes_with_heuristic.append(filtered_data_with_heuristic[i]['runtime'])
            labels_with_heuristic.append(filtered_data_with_heuristic[i]['number_of_generated_labels'])
            runtimes_origin.append(filtered_data_origin[i]['runtime'])
            labels_origin.append(filtered_data_origin[i]['number_of_generated_labels'])

    if len(runtimes_with_heuristic) == 0:
        return
    # 创建一个包含两个子图的图
    fig, axs = plt.subplots(2, 1, figsize=(12, 10))
    
    # 第一个子图：运行时间的柱状图
    bar_width = 0.35
    index = np.arange(len(runtimes_with_heuristic))
    axs[0].bar(index, runtimes_with_heuristic, bar_width, label='With heuristic', align='center', color='blue')
    axs[0].bar(index + bar_width, runtimes_origin, bar_width, label='Origin', align='center', color='red')
    
    axs[0].set_xlabel('Files')
    axs[0].set_ylabel('Runtime (ms)')
    axs[0].set_title('Runtime Comparison for ' + data_type)
    axs[0].legend()
    
    # 第二个子图：标签数量的柱状图
    axs[1].bar(index, labels_with_heuristic, bar_width, label='With heuristic', align='center', color='blue')
    axs[1].bar(index + bar_width, labels_origin, bar_width, label='Origin', align='center', color='red')
    
    axs[1].set_xlabel('Files')
    axs[1].set_ylabel('Number of Generated Labels')
    axs[1].set_title('Label Quantity Comparison for ' + data_type)
    axs[1].legend()

    output_filename = './figures_heuristic/' + data_type + '.png'
    
    # 调整子图间距
    plt.tight_layout()
    plt.savefig(output_filename)
    plt.show()


data_types = ['n20w20', 'n20w40', 'n20w60', 'n20w80', 'n20w100', 'n40w20', 'n40w40', 'n40w60', 'n40w80', 'n40w100', 'n60w20', 'n60w40', 'n60w60', 'n60w80', 'n60w100', 'n80w20', 'n80w40', 'n80w60', 'n80w80', 'n100w20', 'n100w40', 'n100w60', 'n150w20', 'n150w40', 'n150w60', 'n200w20', 'n200w40']
for data_type in data_types:
    plot_data(data_type)