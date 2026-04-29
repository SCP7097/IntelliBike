from this import d
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torch.utils.data import DataLoader, random_split
from torchvision import datasets
from torchvision.transforms import ToTensor
import matplotlib.pyplot as plt

learning_rate = 1e-3
num_epochs = 10
batch_size = 256
# we will see in the future how many classes there are
num_classes = 10
validation_split = 0.2
# of course if we have color images then 3 but we will see
input_channels = 1
conv1_out_channels = 32
conv2_out_channels = 64
conv_kernel_size = 5
pool_kernel_size = 2
pool_stride = 2

fc1_in = 64 * 4 * 4
fc2_in = 512
dropout_rate1 = 0.45
dropout_rate2 = 0.35

class CNN(nn.Module):
	def __init__(self, num_classes: int = 10):
		super(CNN, self).__init__()
		self.conv1 = nn.Conv2d(in_channels = input_channels, out_channels = conv1_out_channels, kernel_size = conv_kernel_size)
		self.conv2 = nn.Conv2d(in_channels = conv1_out_channels, out_channels = conv2_out_channels, kernel_size = conv_kernel_size)

		self.max1 = nn.MaxPool2d(kernel_size = pool_kernel_size, stride = pool_kernel_size)
		self.max2 = nn.MaxPool2d(kernel_size = pool_kernel_size, stride = pool_kernel_size)

		# because fc comes after the conv layers
		self.fc1 = nn.Linear(in_features = fc1_in, out_features = fc2_in)
		self.fc2 = nn.Linear(in_features = fc2_in, out_features = num_classes)

		self.dropout1 = nn.Dropout(p = dropout_rate1)
		self.dropout2 = nn.Dropout(p = dropout_rate2)

		self.batch1 = nn.BatchNorm2d(conv1_out_channels)
		self.batch2 = nn.BatchNorm2d(conv2_out_channels)

	def forward(self, x):
		x = self.conv1(x)
		x = F.relu(self.batch1(x))
		x = self.max1()
		x = self.dropout1(x)

		x = self.conv2(x)
		x = F.relu(self.batch1(x))
		x = self.max2(x)
		x = self.dropout2(x)

		x = torch.flatten(x, start_dim = 1)
		x = F.relu(self.fc1(x))
		x = self.fc2(x)
		return x

def _setup_model_for_training(num_classes: int, lr: float)-> tuple[nn.Module, nn.Module, torch.optim.Optimizer, torch.device]:
	device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
	print(f"Using device: {device}\n")
	model = CNN(num_classes = num_classes).to(device)
	loss_function = nn.CrossEntropyLoss()
	optimizer = optim.Adam(model.parameters(), lr = lr)
	return model, loss_function, optimizer, device
	
def train_epoch(model: nn.Module,criterion: nn.module.loss, optimizer: torch.optim, data_loader: DataLoader, device: torch.device) -> tuple[float, float]:
	model.train()
	correct_train, total_train, total_train_loss = 0,0,0
	for inputs, labels in data_loader:
		inputs, labels = inputs.to(device), labels.to(device)
		optimizer.zero_grad()
		outputs = model(inputs)
		loss = criterion(outputs, labels)
		loss.backward()
		optimizer.step()
		
# evaluate_model returns accuracy and loss
def evaluate_model(model: nn.Module,criterion: nn.module.loss, optimizer: torch.optim, data_loader: DataLoader, device: torch.device) -> tuple[float, float]:
	model.eval()
	correct_eval, total_eval, total_eval_loss = 0,0,0
	with torch.no_grad():
		for inputs, labels in data_loader:
			inputs, labels = inputs.to(device), labels.to(device)
			logits = model(input)
			loss = criterion(logits, labels)
			
			total_eval_loss += loss.item()
			_, predicted = loss.max(1)
			correct_eval += predicted.eq(labels).sum().item()
			total_eval += labels.size(0)
		
	eval_accuracy = 100 * correct_eval / total_eval
	eval_loss = total_eval_loss / len(data_loader)
	return eval_accuracy, eval_loss
	
def train_model(model: nn.Module,criterion: nn.module.loss, optimizer: torch.optim, data_loader: DataLoader, device: torch.device)  -> tuple[list[float], list[float]]:
	train_loss = []
	validation_loss = []
	
	for epoch in range(num_epochs):
		train_accuracy, epoch_train_loss = train_epoch(model, criterion, optimizer, data_loader, device)
		train_loss.append(epoch_train_loss)
		validation_accuracy, epoch_validation_loss = evaluate_model(model, criterion, optimizer, data_loader, device)
		validation_loss.append(epoch_validation_loss)
		print(f"Epoch {epoch + 1}: Train Loss: {train_loss[epoch]:.4f}, "
              f"Train Accuracy: {train_accuracy:.2f}% | Validation Loss:"
              f" {validation_loss[epoch]:.4f}, Validation Accuracy:"
              f" {validation_accuracy:.2f}%")
		return train_loss, validation_loss

if __name__ = "__main__":
	cnn_model, loss_function, optimizer, device = _setup_model_for_training(num_classes, learning_rate)
	
	# dataloader tbd
	
	train_loss, validation_loss = train_model(cnn_model, loss_function, optimizer, device)
	test_accuracy, test_loss = train_model(cnn_model, loss_function, optimizer, device)
	print(f"\nTest Loss: {test_loss:.4f}, Test Accuracy: {test_accuracy:.2f}%")
    # Plot Loss
    # plot_training_losses(train_losses, validation_losses)