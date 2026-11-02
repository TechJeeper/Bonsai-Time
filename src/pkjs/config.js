module.exports = [
  {
    type: 'heading',
    defaultValue: 'Bonsai Time'
  },
  {
    type: 'text',
    defaultValue: 'Your bonsai grows new leaves with every step. Hit your daily goal and it blossoms — then sheds at midnight.'
  },

  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Time'
      },
      {
        type: 'select',
        messageKey: 'TimeFormat',
        defaultValue: '0',
        label: 'Hour format',
        options: [
          { label: 'System default', value: '0' },
          { label: '12-hour', value: '1' },
          { label: '24-hour', value: '2' }
        ]
      },
      {
        type: 'toggle',
        messageKey: 'LeadingZero',
        label: 'Leading zero on hours',
        defaultValue: true
      },
      {
        type: 'toggle',
        messageKey: 'ShowDate',
        label: 'Show date',
        defaultValue: false
      }
    ]
  },

  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Bonsai'
      },
      {
        type: 'slider',
        messageKey: 'StepGoal',
        label: 'Daily step goal (blossom)',
        defaultValue: 8000,
        min: 2000,
        max: 20000,
        step: 500
      },
      {
        type: 'toggle',
        messageKey: 'ShowSteps',
        label: 'Show step count',
        defaultValue: true
      },
      {
        type: 'toggle',
        messageKey: 'ShowStageLabel',
        label: 'Show growth stage',
        defaultValue: true
      },
      {
        type: 'select',
        messageKey: 'BgStyle',
        defaultValue: '0',
        label: 'Background',
        options: [
          { label: 'Zen garden (warm)', value: '0' },
          { label: 'Dawn mist (cool)', value: '1' },
          { label: 'Ink wash (dark)', value: '2' }
        ]
      },
      {
        type: 'toggle',
        messageKey: 'DemoMode',
        label: 'Demo mode (cycle growth stages)',
        defaultValue: false
      }
    ]
  },

  {
    type: 'submit',
    defaultValue: 'Save'
  }
];
