# import matplotlib.pyplot as plt
# import numpy as np

# # 从文本文件中读取数据
# data = np.genfromtxt('/data/cqy/records/sequential.txt', dtype=None, delimiter=' ', names=['prefix', 'virtual_addr', 'timestamps'])

# # 提取地址部分并转换为整数
# addresses = [int(addr.decode('utf-8').split('=')[1], 16) for addr in data['virtual_addr']]

# # 创建时间数据（千分之一秒为间隔）
# time_interval = 0.001  # 1毫秒
# time_points = np.arange(len(addresses)) * time_interval

# # 创建可视化图表
# plt.figure(figsize=(10, 6))
# plt.plot(time_points, addresses)
# plt.xlabel('Time (seconds)')
# plt.ylabel('Address')
# plt.title('Address vs. Time')
# plt.grid(True)

# # 保存图表为文件
# plt.savefig('/data/cqy/records/sequential.png')


# import matplotlib.pyplot as plt
# import numpy as np

# # 从文本文件中读取数据
# data = np.genfromtxt('/data/cqy/records/sequential.txt', dtype=None, delimiter=' ', names=['prefix', 'virtual_addr', 'timestamps'])

# # 提取地址部分并转换为整数
# addresses = [int(addr.decode('utf-8').split('=')[1], 16) for addr in data['virtual_addr']]

# # 计算页面索引并转换为4KB间隔
# page_size_kb = 4
# addresses_in_pages = [(addr // (page_size_kb * 1024)) * (page_size_kb * 1024) for addr in addresses]

# # 创建时间数据（千分之一秒为间隔）
# time_interval = 0.001  # 1毫秒
# time_points = np.arange(len(addresses_in_pages)) * time_interval

# # 创建可视化图表
# plt.figure(figsize=(10, 6))
# plt.plot(time_points, addresses_in_pages)
# plt.xlabel('Time (seconds)')
# plt.ylabel('Address (4KB pages)')
# plt.title('Address vs. Time (4KB Pages)')
# plt.grid(True)

# # 保存图表为文件
# plt.savefig('/data/cqy/records/sequential_with_pages.png')

# import matplotlib.pyplot as plt
# import numpy as np

# # 从文本文件中读取数据
# data = np.genfromtxt('/data/cqy/records/sequential.txt', dtype=None, delimiter=' ', names=['prefix', 'virtual_addr', 'timestamps'])

# # 提取地址部分并转换为整数
# addresses = [int(addr.decode('utf-8').split('=')[1], 16) for addr in data['virtual_addr']]

# # 计算基准地址
# base_address = addresses[0]

# # 计算每个地址相对于基准地址的偏移量并将其转换为4KB页面大小的块
# page_size_bytes = 4 * 1024  * 1024 
# addresses_in_pages = [(addr - base_address) // page_size_bytes for addr in addresses]

# min_address = min(addresses_in_pages)
# addresses_in_pages = [addr - min_address for addr in addresses_in_pages]

# # 创建时间数据（千分之一秒为间隔）,采样频率默认为1000Hz
# time_interval = 0.001  # 1毫秒
# time_points = np.arange(len(addresses_in_pages)) * time_interval

# # 创建可视化图表
# plt.figure(figsize=(10, 6))
# plt.plot(time_points, addresses_in_pages)
# plt.xlabel('Time (seconds)')
# plt.ylabel('Address (4KB pages)')
# plt.title('Address vs. Time (4KB Pages)')
# plt.grid(True)

# # 保存图表为文件
# plt.savefig('/data/cqy/records/sequential_with_pages.png')

# import matplotlib.pyplot as plt
# import numpy as np

# # 从文本文件中读取数据
# data = np.genfromtxt('/data/cqy/records/sequential.txt', dtype=None, delimiter=' ', names=['prefix', 'virtual_addr', 'timestamps'])

# # 提取地址部分并转换为整数
# addresses = [int(addr.decode('utf-8').split('=')[1], 16) for addr in data['virtual_addr']]

# # 计算基准地址
# base_address = addresses[0]

# # 计算每个地址相对于基准地址的偏移量并将其转换为4KB页面大小的块
# page_size_bytes = 4 * 1024
# addresses_in_pages = [(addr - base_address) // page_size_bytes for addr in addresses]

# # 获取最小地址，以便偏移量不为负数
# min_address = min(addresses_in_pages)
# addresses_in_pages = [addr - min_address for addr in addresses_in_pages]

# # 创建时间数据（千分之一秒为间隔），采样频率默认为1000Hz
# time_interval = 0.001  # 1毫秒
# time_points = np.arange(len(addresses_in_pages)) * time_interval

# # 创建可视化图表
# plt.figure(figsize=(10, 6))
# plt.plot(time_points, addresses_in_pages)
# plt.xlabel('Time (seconds)')
# plt.ylabel('Address (4KB pages)')
# plt.title('Address vs. Time (4KB Pages)')
# plt.grid(True)

# # 保存图表为文件
# plt.savefig('/data/cqy/records/sequential_with_pages.png')

# -*- coding: utf-8 -*-

# import matplotlib
# import matplotlib.pyplot as plt
# matplotlib.use('Agg')
# import numpy as np

# # 从文本文件中读取数据
# data = np.genfromtxt('./virtualAddr.txt', dtype=None, delimiter=' ', names=['prefix', 'virtual_addr', 'timestamps'])

# # 提取地址部分并转换为整数
# addresses = [int(addr.decode('utf-8').split('=')[1], 16) for addr in data['virtual_addr']]
# #sed -i '/addr=7f/!d' your_file.txt
# #删除不以7f开头的

# # 计算基准地址
# base_address = addresses[0]

# # 获取最小地址，以便偏移量不为负数
# min_address = min(addresses)

# # 计算每个地址相对于基准地址的偏移量并将其转换为4KB页面大小的块
# page_size_bytes = 4 * 1024 * 1024 
# addresses_in_pages = [(addr - min_address) // page_size_bytes for addr in addresses]

# # 创建时间数据（千分之一秒为间隔），采样频率默认为1000Hz
# time_interval = 0.001  # 1毫秒
# time_points = np.arange(len(addresses_in_pages)) * time_interval

# # 创建可视化图表
# plt.figure(figsize=(10, 6))
# plt.plot(time_points, addresses_in_pages)
# plt.xlabel('Time (seconds)')
# plt.ylabel('Address (4KB pages)')
# plt.title('Address vs. Time (4KB Pages)')
# plt.grid(True)

# # 保存图表为文件
# plt.savefig('./gemm_pattern.png')

#散点图的
# import matplotlib
# import matplotlib.pyplot as plt
# import numpy as np

# # 设置字体属性
# font = {'family': 'serif',
#         'color': 'black',
#         'weight': 'normal',
#         'size': 14,
#         }

# matplotlib.use('Agg')

# # 从文本文件中读取数据
# data = np.genfromtxt('./virtualAddr.txt', dtype=None, delimiter=' ', names=['prefix', 'virtual_addr', 'timestamps'])

# # 提取地址部分并转换为整数
# addresses = [int(addr.decode('utf-8').split('=')[1], 16) for addr in data['virtual_addr']]

# # 计算基准地址
# base_address = addresses[0]

# # 获取最小地址，以便偏移量不为负数
# min_address = min(addresses)

# # 计算每个地址相对于基准地址的偏移量并将其转换为4KB页面大小的块
# page_size_bytes = 4 * 1024 * 1024
# addresses_in_pages = [(addr - min_address) // page_size_bytes for addr in addresses]

# # 创建时间数据（千分之一秒为间隔），采样频率默认为1000Hz
# time_interval = 0.001  # 1毫秒
# time_points = np.arange(len(addresses_in_pages)) * time_interval

# # 创建可视化图表
# plt.figure(figsize=(10, 6))

# # 使用scatter函数绘制散点图
# plt.scatter(time_points, addresses_in_pages, s=5, alpha=0.5, c='b')

# # 设置X轴和Y轴标签及标题的字体属性
# plt.xlabel('Time (seconds)', fontdict=font)
# plt.ylabel('Address (4KB pages)', fontdict=font)
# plt.title('Address vs. Time (4KB Pages)', fontdict=font)

# plt.grid(False)

# # 保存图表为文件
# plt.savefig('./gemm_pattern.png')




#原始的
# import matplotlib
# import matplotlib.pyplot as plt
# import numpy as np

# # 设置字体属性
# font = {'family': 'serif',
#         'color':  'black',
#         'weight': 'normal',
#         'size': 14,
#         }

# matplotlib.use('Agg')

# # 从文本文件中读取数据
# data = np.genfromtxt('./virtualAddr.txt', dtype=None, delimiter=' ', names=['prefix', 'virtual_addr', 'timestamps'])

# # 提取地址部分并转换为整数
# addresses = [int(addr.decode('utf-8').split('=')[1], 16) for addr in data['virtual_addr']]

# # 计算基准地址
# base_address = addresses[0]

# # 获取最小地址，以便偏移量不为负数
# min_address = min(addresses)

# # 计算每个地址相对于基准地址的偏移量并将其转换为4KB页面大小的块
# page_size_bytes = 4 * 1024 * 1024
# addresses_in_pages = [(addr - min_address) // page_size_bytes for addr in addresses]

# # 创建时间数据（千分之一秒为间隔），采样频率默认为1000Hz
# time_interval = 0.001  # 1毫秒
# time_points = np.arange(len(addresses_in_pages)) * time_interval

# # 创建可视化图表
# plt.figure(figsize=(10, 6))
# plt.plot(time_points, addresses_in_pages)

# # 设置X轴和Y轴标签及标题的字体属性
# plt.xlabel('Time (seconds)', fontdict=font)
# plt.ylabel('Address (4KB pages)', fontdict=font)
# plt.title('Address vs. Time (4KB Pages)', fontdict=font)

# plt.grid(False)

# # 保存图表为文件
# plt.savefig('./gemm_pattern.png')


#加入对频率的可视化
# import matplotlib
# import matplotlib.pyplot as plt
# import numpy as np

# # 设置字体属性
# font = {'family': 'serif',
#         'color':  'black',
#         'weight': 'normal',
#         'size': 14,
#         }

# matplotlib.use('Agg')

# # 从文本文件中读取数据
# data = np.genfromtxt('./virtualAddr.txt', dtype=None, delimiter=' ', names=['prefix', 'virtual_addr', 'timestamps'])

# # 提取地址部分并转换为整数
# addresses = [int(addr.decode('utf-8').split('=')[1], 16) for addr in data['virtual_addr']]

# # 计算基准地址
# base_address = addresses[0]

# # 获取最小地址，以便偏移量不为负数
# min_address = min(addresses)

# # 计算每个地址相对于基准地址的偏移量并将其转换为4KB页面大小的块
# page_size_bytes = 4 * 1024 * 1024
# addresses_in_pages = [(addr - min_address) // page_size_bytes for addr in addresses]

# # 统计每个地址的访问次数
# address_counts = {}
# for address in addresses_in_pages:
#     if address in address_counts:
#         address_counts[address] += 1
#     else:
#         address_counts[address] = 1

# # 创建时间数据（千分之一秒为间隔），采样频率默认为1000Hz
# time_interval = 0.001  # 1毫秒
# time_points = np.arange(len(addresses_in_pages)) * time_interval

# # 创建可视化图表
# plt.figure(figsize=(10, 6))

# # 映射访问次数到颜色和点大小
# colors = [address_counts[address] for address in addresses_in_pages]
# sizes = [address_counts[address] * 2 for address in addresses_in_pages]  # 根据访问次数调整点的大小

# # 绘制散点图
# plt.scatter(time_points, addresses_in_pages, c=colors, s=sizes, cmap='viridis', alpha=0.5)

# # 设置X轴和Y轴标签及标题的字体属性
# plt.xlabel('Time (seconds)', fontdict=font)
# plt.ylabel('Address (4KB pages)', fontdict=font)
# plt.title('Address vs. Time (4KB Pages)', fontdict=font)

# plt.grid(False)

# # 添加颜色条
# plt.colorbar()

# # 保存图表为文件
# plt.savefig('./gemm_pattern.png')





#离散形式
# import matplotlib
# import matplotlib.pyplot as plt
# import numpy as np

# # 设置字体属性
# font = {'family': 'serif',
#         'color':  'black',
#         'weight': 'normal',
#         'size': 14,
#         }

# matplotlib.use('Agg')

# # 从文本文件中读取数据
# data = np.genfromtxt('./virtualAddr.txt', dtype=None, delimiter=' ', names=['prefix', 'virtual_addr', 'timestamps'])

# # 提取地址部分并转换为整数
# addresses = [int(addr.decode('utf-8').split('=')[1], 16) for addr in data['virtual_addr']]

# # 计算基准地址
# base_address = addresses[0]

# # 获取最小地址，以便偏移量不为负数
# min_address = min(addresses)

# # 计算每个地址相对于基准地址的偏移量并将其转换为4KB页面大小的块
# page_size_bytes = 4 * 1024 * 1024
# addresses_in_pages = [(addr - min_address) // page_size_bytes for addr in addresses]

# # 统计每个地址的访问次数
# address_counts = {}
# for address in addresses_in_pages:
#     if address in address_counts:
#         address_counts[address] += 1
#     else:
#         address_counts[address] = 1

# # 创建时间数据（千分之一秒为间隔），采样频率默认为1000Hz
# time_interval = 0.001  # 1毫秒
# time_points = np.arange(len(addresses_in_pages)) * time_interval

# # 创建可视化图表
# plt.figure(figsize=(10, 6))

# # 映射访问次数到颜色
# colors = [address_counts[address] for address in addresses_in_pages]

# # 创建一个包含每次内存访问的离散点
# plt.scatter(time_points, addresses_in_pages, c=colors, cmap='viridis', s=2, alpha=0.5)

# # 设置X轴和Y轴标签及标题的字体属性
# plt.xlabel('Time (seconds)', fontdict=font)
# plt.ylabel('Address (4KB pages)', fontdict=font)
# plt.title('Discrete Memory Access Points (Color-coded by Access Count)', fontdict=font)

# plt.grid(False)

# # 添加颜色条
# plt.colorbar()

# # 保存图表为文件
# plt.savefig('./gemm_pattern.png')



# import matplotlib
# import matplotlib.pyplot as plt
# import numpy as np

# # 设置字体属性
# font = {'family': 'serif',
#         'color':  'black',
#         'weight': 'normal',
#         'size': 14,
#         }

# matplotlib.use('Agg')

# # 从文本文件中读取数据
# data = np.genfromtxt('./virtualAddr.txt', dtype=None, delimiter=' ', names=['prefix', 'virtual_addr', 'timestamps'])

# # 提取地址部分并转换为整数
# addresses = [int(addr.decode('utf-8').split('=')[1], 16) for addr in data['virtual_addr']]

# # 计算基准地址
# base_address = addresses[0]

# # 获取最小地址，以便偏移量不为负数
# min_address = min(addresses)

# # 计算每个地址相对于基准地址的偏移量并将其转换为4KB页面大小的块
# page_size_bytes = 4 * 1024 * 1024
# addresses_in_pages = [(addr - min_address) // page_size_bytes for addr in addresses]

# # 统计每个地址的访问次数
# address_counts = {}
# for address in addresses_in_pages:
#     if address in address_counts:
#         address_counts[address] += 1
#     else:
#         address_counts[address] = 1

# # 创建时间数据（千分之一秒为间隔），采样频率默认为1000Hz
# time_interval = 0.001  # 1毫秒
# time_points = np.arange(len(addresses_in_pages)) * time_interval

# # 创建可视化图表
# plt.figure(figsize=(10, 6))

# # 映射访问次数到颜色
# colors = [address_counts[address] for address in addresses_in_pages]

# # 创建一个包含每次内存访问的离散点，颜色深浅根据访问次数映射
# plt.scatter(time_points, addresses_in_pages, c=colors, cmap='viridis', s=2, alpha=0.5)

# # 设置X轴和Y轴标签及标题的字体属性
# plt.xlabel('Time (seconds)', fontdict=font)
# plt.ylabel('Address (4KB pages)', fontdict=font)
# plt.title('Discrete Memory Access Points (Color-coded by Access Count)', fontdict=font)

# plt.grid(False)

# # 添加颜色条
# plt.colorbar(label='Access Count')

# # 保存图表为文件
# plt.savefig('./gemm_pattern.png')

#可视化时间太长
# import matplotlib.pyplot as plt
# import numpy as np
# import matplotlib

# # 设置字体属性
# font = {'family': 'serif',
#         'color':  'black',
#         'weight': 'normal',
#         'size': 14,
#         }

# matplotlib.use('Agg')

# # 从文本文件中读取数据
# data = np.genfromtxt('./virtualAddr.txt', dtype=None, delimiter=' ', names=['prefix', 'virtual_addr', 'timestamps'])

# # 提取地址部分并转换为整数
# addresses = [int(addr.decode('utf-8').split('=')[1], 16) for addr in data['virtual_addr']]

# # 计算基准地址
# base_address = addresses[0]

# # 获取最小地址，以便偏移量不为负数
# min_address = min(addresses)

# # 计算每个地址相对于基准地址的偏移量并将其转换为4KB页面大小的块
# page_size_bytes = 4 * 1024 * 1024
# addresses_in_pages = [(addr - min_address) // page_size_bytes for addr in addresses]

# # 统计每个地址的访问次数
# address_counts = {}
# for address in addresses_in_pages:
#     if address in address_counts:
#         address_counts[address] += 1
#     else:
#         address_counts[address] = 1

# # 将访问次数为零的地址也添加到访问次数字典中
# for address in range(max(addresses_in_pages) + 1):
#     if address not in address_counts:
#         address_counts[address] = 0

# # 创建时间数据（千分之一秒为间隔），采样频率默认为1000Hz
# time_interval = 0.001  # 1毫秒
# time_points = np.arange(len(addresses_in_pages)) * time_interval

# # 使用对数尺度来调整颜色映射
# log_counts = [np.log(count) if count > 0 else 0 for count in address_counts.values()]

# # 自定义颜色映射，将低访问次数映射到浅色，高访问次数映射到深色
# cmap = plt.get_cmap('viridis')
# norm = matplotlib.colors.Normalize(vmin=min(log_counts), vmax=max(log_counts))
# colors = [cmap(norm(log_count)) for log_count in log_counts]

# # 创建可视化图表
# plt.figure(figsize=(10, 6))

# # 创建一个包含每次内存访问的离散点，颜色深浅根据访问次数映射
# plt.scatter(time_points, addresses_in_pages, c=colors, s=1, alpha=0.1)

# # 设置X轴和Y轴标签及标题的字体属性
# plt.xlabel('Time (seconds)', fontdict=font)
# plt.ylabel('Address (4KB pages)', fontdict=font)
# plt.title('Discrete Memory Access Points (Color-coded by Access Count)', fontdict=font)

# plt.grid(False)

# # 添加颜色条
# sm = plt.cm.ScalarMappable(cmap=cmap, norm=norm)
# sm.set_array([])
# plt.colorbar(sm, label='Log Access Count')

# # 保存图表为文件
# plt.savefig('./gemm_pattern.png')








import matplotlib.pyplot as plt
import numpy as np
import matplotlib

# 设置字体属性
font = {'family': 'serif',
        'color': 'black',
        'weight': 'normal',
        'size': 14,
        }

matplotlib.use('Agg')

# 从文本文件中读取数据
data = np.genfromtxt('./virtualAddr.txt', dtype=None, delimiter=' ', names=['prefix', 'virtual_addr', 'timestamps'])

# 提取地址部分并转换为整数
addresses = [int(addr.decode('utf-8').split('=')[1], 16) for addr in data['virtual_addr']]

# 计算基准地址
base_address = addresses[0]

# 获取最小地址，以便偏移量不为负数
min_address = min(addresses)

# 计算每个地址相对于基准地址的偏移量并将其转换为4KB页面大小的块
page_size_bytes = 4 * 1024 
addresses_in_pages = [(addr - min_address) // page_size_bytes for addr in addresses]

# 统计每个页面的访问次数
page_counts = {}
for page in addresses_in_pages:
    if page in page_counts:
        page_counts[page] += 1
    else:
        page_counts[page] = 1

# 最大访问次数，用于归一化颜色映射
max_count = max(page_counts.values())

# 创建时间数据（千分之一秒为间隔），采样频率默认为1000Hz
time_interval = 0.001  # 1毫秒
time_points = np.arange(len(addresses_in_pages)) * time_interval

# 使用对数尺度来调整颜色映射
log_counts = [np.log(count) if count > 0 else 0 for count in page_counts.values()]

# 自定义颜色映射，将低访问次数映射到浅色，高访问次数映射到深色
cmap = plt.get_cmap('viridis')
norm = matplotlib.colors.Normalize(vmin=0, vmax=max_count)
colors = [cmap(norm(page_counts[page])) for page in addresses_in_pages]

# 创建可视化图表
plt.figure(figsize=(10, 6))

# 创建一个包含每个页面内存访问的离散点，颜色深浅根据页面访问次数映射
plt.scatter(time_points, addresses_in_pages, c=colors, s=1, alpha=0.1)
# plt.plot(time_points, addresses_in_pages)

# 设置X轴和Y轴标签及标题的字体属性
plt.xlabel('Time (seconds)', fontdict=font)
plt.ylabel('Page', fontdict=font)
plt.title('Memory Pages Access (Color-coded by Access Count)', fontdict=font)

plt.grid(False)

# 添加颜色条
sm = plt.cm.ScalarMappable(cmap=cmap, norm=norm)
sm.set_array([])
plt.colorbar(sm, label='Log Access Count')

# 保存图表为文件
plt.savefig('./kmeans_pattern.png')
