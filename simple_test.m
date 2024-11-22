axon = nrn_section_new('axon4');
nrn_section_push(axon);
nrn_section_length_get(axon);

% Tests for section_length functions
length_of_axon = nrn_section_length_get(axon); % Capture the length of the axon
disp(['Axon4 Length: ', num2str(length_of_axon)]); % Display the length
nrn_section_length_set(axon, 20.2); % Set the length of the axon
length_of_axon = nrn_section_length_get(axon); % Capture the length of the axon
disp(['Length of Inputted Axon: ', num2str(length_of_axon)]); % Display the length

% Tests for nseg functions
nseg_axon = nrn_nseg_get(axon); % Capture nseg of the axon
disp(['Nseg of axon: ', num2str(nseg_axon)]); % Display nseg
nrn_nseg_set(axon, 17.5);
nseg_axon = nrn_nseg_get(axon); % Capture nseg of the axon
disp(['Nseg of axon after setting: ', num2str(nseg_axon)]); % Display nseg

% Tests for RA functions
ra_axon = nrn_section_Ra_get(axon); % Capture RA of the axon
disp(['RA axon: ', num2str(ra_axon)]); % Display RA

nrn_section_Ra_set(axon, 24.5);
ra_axon = nrn_section_Ra_get(axon); % Capture RA of the axon
disp(['RA axon after setting: ', num2str(ra_axon)]); % Display RA

x = 0.5; % Position along the section (0 to 1)
diam = 1.5; % Diameter to set
nrn_segment_diam_set(axon, x, diam); % Set the diameter of the segment

% Create and connect new sections
parent_sec = nrn_section_new('parent');
child_sec = nrn_section_new('child');

disp('Connecting sections...');

% Safely connect sections, avoiding loops
nrn_section_connect(child_sec, 0, parent_sec, 1); % Connect child to parent
nrn_section_connect(axon, 0, parent_sec, 1); % Connect axon to parent (no loops)

disp('Testing section connections:');
nrn_hoc_call('forall psection()'); %3 Print all sections and connections

% Helper Functions
function sec = nrn_section_new(name)
  sec = neuron_api('nrn_section_new', name);
end

function nrn_hoc_call(command)
  neuron_api('nrn_hoc_call', command);
end

function nrn_section_push(sec)
  neuron_api('nrn_section_push', sec);
end

function nrn_nseg_set(sec, nseg)
  neuron_api('nrn_nseg_set', sec, nseg);
end

function nsegment = nrn_nseg_get(sec)
  nsegment = neuron_api('nrn_nseg_get', sec); % Retrieve and return nseg
end

function nrn_section_Ra_set(sec, ra)
  neuron_api('nrn_section_Ra_set', sec, ra);
end

function ra_length = nrn_section_Ra_get(sec)
  ra_length = neuron_api('nrn_section_Ra_get', sec); % Retrieve and return RA
end

function nrn_section_length_set(sec, L)
  neuron_api('nrn_section_length_set', sec, L);
end

function length = nrn_section_length_get(sec)
  length = neuron_api('nrn_section_length_get', sec); % Retrieve and return length
end

function name = nrn_secname(sec)
  name = neuron_api('nrn_secname', sec); % Get the name of the section
end

function nrn_segment_diam_set(sec, x, diam)
  neuron_api('nrn_segment_diam_set', sec, x, diam);
end

function diam = nrn_segment_diam_get(sec, x)
  diam = neuron_api('nrn_segment_diam_get', sec, x);
end

function nrn_rangevar_set(sec, value)
  neuron_api('nrn_rangevar_set', sec, value);
end

function value = nrn_rangevar_get(sec)
  value = neuron_api('nrn_rangevar_get', sec);
end

function nrn_section_pop()
  neuron_api('nrn_section_pop'); % Pop the current section
end

function nrn_section_connect(child_sec, child_x, parent_sec, parent_x)
  % Connect the child section to the parent section
  neuron_api('nrn_section_connect', child_sec, child_x, parent_sec, parent_x);
end

